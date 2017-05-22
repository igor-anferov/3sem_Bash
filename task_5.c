/* task 5 - "My Shell" */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#ifdef __linux__
#include <linux/types.h>
#endif
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "wordlist.h" /* getlist, dellist */
#include "runtree.h"

typedef struct Context {
    int PID;
    char **argv;
    struct Context *next;
} Context;

int stdinFD, stdoutFD;      /* сохранённые дескрипторы стандартных IO-потоков */
char *HOME;                 /* домашняя директория пользователя */
char WD[PATH_MAX+1];        /* рабочая директория */
char SHELL[PATH_MAX+1];     /* путь к исполняемому в данный момент шеллу */
char *USER;                 /* имя пользователя, запустившего процесс */
unsigned long EUID;         /* идентификатор пользователя, с правами которого
                             работает процесс */

/* Обработчик сигнала SIGINT */
void sigintHandler(int sig) {
    signal(SIGINT, SIG_IGN);
    kill(0,SIGINT);
    signal(SIGINT, sigintHandler);
}

/* Функция добавляет процесс к списку фоновых процессов */
void addBack(Context **c, int PID, char **argv) {
    while (*c!=NULL)
        c=&((*c)->next);
    *c=malloc(sizeof(Context));
    (*c)->next=NULL;
    (*c)->PID=PID;
    (*c)->argv=argv;
}

/* Функция проверяет список фоновых процессов и выводит информацию о завершившихся */
void clearBack(Context **c, int waitExit) {
    Context *tmp;
    int status, ret;
    unsigned long i;
    while (*c!=NULL) {
        ret=waitpid((*c)->PID, &status, waitExit?0:(WNOHANG));
        if (ret) {
            if (ret>0 && waitExit!=3 && (*c)->argv) {
                if (WIFEXITED(status))
                    printf ("Background process [%d] (%s) was exited with exit code %d\n",
                            (*c)->PID,(*c)->argv[0],WEXITSTATUS(status));
                else
                    if (WIFSIGNALED(status))
                        printf ("Background process [%d] (%s) was terminated by signal %d\n",
                                (*c)->PID,(*c)->argv[0],WTERMSIG(status));
                    else
                        printf ("Background process [%d] (%s) isn't alive any more\n",
                                (*c)->PID,(*c)->argv[0]);
                for (i=0; (*c)->argv[i]!=NULL; i++)
                    free((*c)->argv[i]);
            }
            free((*c)->argv);
            tmp=*c;
            *c=(*c)->next;
            free(tmp);
        }
        else
            c=&((*c)->next);
    }
    return;
}

/* Функция отправляет SIGKILL всем процессам из списка фоновых */
void killBack(Context *c) {
    while (c!=NULL) {
        kill(c->PID, SIGKILL);
        c=c->next;
    }
}

/* Функция подставляет во всех словах списка (wl) значения переменных окружения */
void variablesChg(WordList *wl) {
    unsigned long newsize;
    char *ret, *tmp, EUIDstr[34];
    sprintf(EUIDstr, "%lu", EUID);
    while (wl!=NULL) {
        while ((ret=strstr(wl->word, "$HOME"))) {
            newsize=strlen(wl->word)+strlen(HOME)+1;
            ret[0]='\0';
            tmp=calloc(newsize,sizeof(char));
            wl->word=realloc(wl->word,newsize*sizeof(char));
            strcat(tmp,HOME);
            strcat(tmp,&(ret[5]));
            strcat(wl->word,tmp);
            free(tmp);
        }
        while ((ret=strstr(wl->word, "$SHELL"))) {
            newsize=strlen(wl->word)+strlen(SHELL)+1;
            ret[0]='\0';
            tmp=calloc(newsize,sizeof(char));
            wl->word=realloc(wl->word,newsize*sizeof(char));
            strcat(tmp,SHELL);
            strcat(tmp,&(ret[6]));
            strcat(wl->word,tmp);
            free(tmp);
        }
        while ((ret=strstr(wl->word, "$USER"))) {
            newsize=strlen(wl->word)+strlen(USER)+1;
            ret[0]='\0';
            tmp=calloc(newsize,sizeof(char));
            wl->word=realloc(wl->word,newsize*sizeof(char));
            strcat(tmp,USER);
            strcat(tmp,&(ret[5]));
            strcat(wl->word,tmp);
            free(tmp);
        }
        while ((ret=strstr(wl->word, "$EUID"))) {
            newsize=strlen(wl->word)+strlen(EUIDstr)+1;
            ret[0]='\0';
            tmp=calloc(newsize,sizeof(char));
            wl->word=realloc(wl->word,newsize*sizeof(char));
            strcat(tmp,EUIDstr);
            strcat(tmp,&(ret[5]));
            strcat(wl->word,tmp);
            free(tmp);
        }
        wl=wl->next;
    }
}

/* Функция запускает структуру процессов, описанную деревом RT, дерево при
 этом уничтожается (за исключением argv фоновых процессов, они перевешиваются
 в структуру BP и затем их очищает функция clearBack) */
int run(RunTree *RT, Context **BP) {
    Context *PipelineProcs=NULL;
    int localStdinFD=stdinFD, localStdoutFD=stdoutFD;
    int masterPID, PID;
    int tmpFD;
    int pipes[]={0,0};
    int status;
    int back=0;
    
    masterPID=getpid();
    if (RT==NULL)
        return 0;
    while (1) {
        if (pipes[0]) {
            dup2(pipes[0],0);
            close(pipes[0]);
            pipes[0]=0;
        } else
            if (RT->infile) {
                if ((tmpFD=open(RT->infile, O_RDONLY, 0))==-1) {
                    fprintf(stderr,
                            "Can't find input file.\n");
                    return -3;
                }
                dup2(tmpFD,0);
                close(tmpFD);
            } else
                if (RT->backgrnd) {
                    tmpFD=open("/dev/null", O_RDONLY, 0);
                    dup2(tmpFD,0);
                    close(tmpFD);
                }
                else
                    dup2(localStdinFD,0);
        
        if (RT->outfile) {
            tmpFD=open(RT->outfile,
                       (RT->append)?(O_CREAT | O_APPEND | O_RDWR):(O_CREAT | O_WRONLY | O_TRUNC), 0666);
            dup2(tmpFD,1);
            close(tmpFD);
            if (RT->pipe && !RT->pipe->infile) {
                RT->pipe->infile=malloc(sizeof(char)*(strlen("/dev/null")+1));
                strcpy(RT->pipe->infile,"/dev/null");
            }
        } else
            if (RT->pipe) {
                pipe(pipes);
                dup2(pipes[1],1);
                close(pipes[1]);
                pipes[1]=0;
            } else
                dup2(localStdoutFD,1);
        
        if ((RT->next && RT->pipe) || RT->psubcmd || (RT->argv && strcmp(RT->argv[0],"cd") &&
                                                      strcmp(RT->argv[0],"exit"))) {
            if ((PID=fork())==0) {
                if (pipes[0])
                    close(pipes[0]);
                if (pipes[1])
                    close(pipes[1]);
                pipes[0]=0;
                pipes[1]=0;
                if (RT->next && RT->pipe) {
                    RT->next=NULL;
                    continue;
                }
                if (RT->backgrnd || back) {
                    back=1;
                    signal(SIGINT, SIG_IGN);
                }
                else
                    signal(SIGINT, SIG_DFL);
                if (RT->argv && execvp(RT->argv[0], RT->argv)==-1) {
                    fprintf(stderr, "EXEC failed.\n");
                    return -1;
                }
                localStdinFD=dup(0);
                localStdoutFD=dup(1);
                RT=RT->psubcmd;
                continue;
            }
            else
                if (PID==-1) {
                    fprintf(stderr,
                            "Wncorrectable error. FORK failed. Shell exiting now.\n");
                    return -1;
                }
        }
        
        if (!RT->pipe && RT->argv && !strcmp(RT->argv[0],"cd") && chdir(RT->argv[1]?RT->argv[1]:HOME))
            printf ("Directory changing failed\n");
        
        if (!RT->pipe && RT->argv && !strcmp(RT->argv[0],"exit"))
            return -1;
        
        if (RT->next && RT->pipe) {
            RT->pipe=NULL;
            RT->argv=NULL;
        }
        
        dup2(localStdinFD,0);
        dup2(localStdoutFD,1);
        
        if (getpid()==masterPID && (RT->backgrnd || back) && (!RT->argv || (RT->argv && strcmp(RT->argv[0],"cd") && strcmp(RT->argv[0],"exit")))) {
            addBack(BP,PID,RT->argv);
            if (RT->pipe)
                back=1;
            RT->argv=NULL;
        }
        else
            if (getpid()==masterPID && (RT->pipe) && !(RT->backgrnd) && !back && (RT->argv && strcmp(RT->argv[0],"cd") && strcmp(RT->argv[0],"exit"))) {
                addBack(&PipelineProcs,PID,RT->argv);
                RT->argv=NULL;
            }
            else
                if ((RT->argv && strcmp(RT->argv[0],"cd")) || !RT->argv)
                    waitpid(PID,&status,0);
        
        if (RT->pipe) {
            RT=RT->pipe;
            continue;
        } else {
            clearBack(&PipelineProcs,3);
            PipelineProcs=NULL;
            back=0;
            if (RT->next && (RT->logical==0 ||
                             (RT->logical==1 && WIFEXITED(status) && WEXITSTATUS(status)==0) ||
                             (RT->logical==2 && (!WIFEXITED(status) || WEXITSTATUS(status)!=0)))) {
                RT=RT->next;
                continue;
            } else
                break;
        }
    }
    if (getpid()!=masterPID)
        return -2;
    dup2(localStdinFD,0);
    dup2(localStdoutFD,1);
    return 0;
}

int main(int argc, char **argv, char **envp) {
    
    RunTree *RT=NULL;
    WordList *wl=NULL;
    Context *backProcs=NULL;
    int ret;
    
    struct passwd *userInfo;
    
    signal(SIGINT, sigintHandler);
    stdinFD=dup(0);
    stdoutFD=dup(1);
    
    /* Заполнение переменных окружения */
    EUID=geteuid();
    userInfo=getpwuid(EUID);
    USER=userInfo->pw_name;
    HOME=userInfo->pw_dir;
    SHELL[0]=0;
    strcat(SHELL, getenv("PWD"));
    strcat(SHELL, "/");
    strcat(SHELL, &(argv[0][argv[0][0]=='.'?2:0]));
    chdir(HOME);
    
    do {
        printf ("%s: %s$ ",getwd(WD),USER);
        ret=getlist(&wl);
        if (ret==0 && wl==NULL) {
            clearBack(&backProcs,0);
            continue;
        }
        if (ret==-1) {
            printf ("Error. Undeclarated symbols are detected\n");
            wl=NULL;
            ret=0;
            continue;
        }
        if (ret==0 && wl!=NULL) {
            variablesChg (wl);
            if (getShellCommand (&RT, &wl)) {
                printf("Syntaxis error!\n");
                delRunTree (&RT);
            }
            ret=run(RT, &backProcs);
            if (ret==-3)
                kill(0,SIGINT);
            delRunTree (&RT);
            dellist(wl);
            if (ret!=-2)
                clearBack(&backProcs,0);
            wl=NULL;
        }
    } while (ret==0 || ret==-3);
    
    if (ret==1)
        printf ("\n");
    if (ret!=-2) {
        killBack(backProcs);
        clearBack(&backProcs,1);
    }
    return 0;
}
