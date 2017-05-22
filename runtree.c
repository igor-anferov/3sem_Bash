/* Модуль содержит функции для работы с деревом запускаемых процессов */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include "wordlist.h" /* getlist, dellist */
#include "runtree.h"

int getShellCommand (RunTree **RT, WordList **WL) {
    int ret=0;
    char *tmp;
    if ((tmp=getlex(WL))==NULL) {
        RT=NULL;
    }
    else {
        ungetlex(WL, tmp);
        tmp=NULL;
        if (getCommandList (RT,WL) || ((tmp=getlex(WL))!=NULL && strcmp(tmp, "#")))
            ret=-1;
        free(tmp);
        dellist(*WL);
        *WL=NULL;
    }
    return ret;
}

int getCommandList (RunTree **RT, WordList **WL) {
    char *tmp1, *tmp2;
    while (1) {
        if (getPipeline(RT, WL))
            return -1;
        if ((tmp1=getlex(WL))!=NULL) {
            if (!strcmp(tmp1,"&"))
                (*RT)->backgrnd=1;
            else
                if (!strcmp(tmp1,"&&")) {
                    (*RT)->logical=1;
                    free (tmp1);
                    RT=&((*RT)->next);
                    continue;
                }
                else
                    if (!strcmp(tmp1,"||")) {
                        (*RT)->logical=2;
                        free (tmp1);
                        RT=&((*RT)->next);
                        continue;
                    }
                    else
                        if (!strcmp(tmp1,"#") || !strcmp(tmp1,")")) {
                            ungetlex (WL,tmp1);
                            return 0;
                        }
                        else
                            if (strcmp(tmp1,";")) {
                                free (tmp1);
                                return -1;
                            }
        }
        else
            return 0;
        free (tmp1);
        if ((tmp2=getlex(WL))==NULL)
            return 0;
        ungetlex (WL,tmp2);
        if (!strcmp(tmp2,"#") || !strcmp(tmp2,")"))
            return 0;
        RT=&((*RT)->next);
    }
}

int getPipeline (RunTree **RT, WordList **WL) {
    char *tmp;
    while (1) {
        if (getCommand(RT, WL))
            return -1;
        if ((tmp=getlex(WL))==NULL)
            return 0;
        if (strcmp(tmp,"|")) {
            ungetlex(WL,tmp);
            return 0;
        }
        free(tmp);
        RT=&((*RT)->pipe);
    }
}

int getCommand (RunTree **RT, WordList **WL) {
    char *tmp;
    if ((tmp=getlex(WL))==NULL)
        return -1;
    if (strcmp(tmp,"(")) {
        ungetlex(WL,tmp);
        return getSimpleCommand (RT, WL);
    }
    free(tmp);
    *RT=malloc(sizeof(RunTree));
    (*RT)->argv=NULL;
    (*RT)->next=NULL;
    (*RT)->pipe=NULL;
    (*RT)->infile=NULL;
    (*RT)->outfile=NULL;
    (*RT)->append=0;
    (*RT)->backgrnd=0;
    (*RT)->logical=0;
    (*RT)->psubcmd=NULL;
    if (getCommandList (&((*RT)->psubcmd), WL))
        return -1;
    if ((tmp=getlex(WL))==NULL)
        return -1;
    if (strcmp(tmp,")")) {
        free(tmp);
        return -1;
    }
    free(tmp);
    return getIO (RT, WL);
}

int getSimpleCommand (RunTree **RT, WordList **WL) {
    *RT=malloc(sizeof(RunTree));
    (*RT)->argv=NULL;
    (*RT)->next=NULL;
    (*RT)->pipe=NULL;
    (*RT)->infile=NULL;
    (*RT)->outfile=NULL;
    (*RT)->append=0;
    (*RT)->backgrnd=0;
    (*RT)->logical=0;
    (*RT)->psubcmd=NULL;
    getArguments (RT, WL);
    if ((*RT)->argv==NULL)
        return -1;
    return getIO (RT, WL);
}

int getIO (RunTree **RT, WordList **WL) {
    char *tmp;
    int i;
    for (i=0; i<2; i++) {
        if ((tmp=getlex(WL))==NULL)
            return 0;
        if (!strcmp(tmp,">") || !strcmp(tmp,">>")) {
            if (!strcmp(tmp,">>"))
                (*RT)->append=1;
            free (tmp);
            if ((tmp=getlex(WL))==NULL || (*RT)->outfile!=NULL) {
                free (tmp);
                return -1;
            }
            (*RT)->outfile=tmp;
        }
        else
            if (!strcmp(tmp,"<")) {
                free (tmp);
                if ((tmp=getlex(WL))==NULL || (*RT)->infile!=NULL) {
                    free (tmp);
                    return -1;
                }
                (*RT)->infile=tmp;
            }
            else
                ungetlex(WL, tmp);
    }
    return 0;
}

void getArguments (RunTree **RT, WordList **WL) {
    char *tmp;
    unsigned long i=0;
    while (1) {
        if ((tmp=getlex(WL))==NULL)
            return;
        if (!strcmp(tmp,">") || !strcmp(tmp,">>") || !strcmp(tmp,";") ||
            !strcmp(tmp,"&") || !strcmp(tmp,"&&") || !strcmp(tmp,"(") ||
            !strcmp(tmp,"|") || !strcmp(tmp,"||") || !strcmp(tmp,")") ||
            !strcmp(tmp,"<") || !strcmp(tmp,"#")) {
            ungetlex(WL, tmp);
            return;
        }
        (*RT)->argv=realloc((*RT)->argv,(i+2)*sizeof(char *));
        ((*RT)->argv)[i]=tmp;
        ((*RT)->argv)[i+1]=NULL;
        i++;
    }
}

void delRunTree (RunTree **RT) {
    unsigned long i;
    if (*RT!=NULL) {
        if ((*RT)->argv!=NULL) {
            for (i=0; (*RT)->argv[i]!=NULL; i++)
                free((*RT)->argv[i]);
            free((*RT)->argv);
            (*RT)->argv=NULL;
        }
        delRunTree (&((*RT)->next));
        delRunTree (&((*RT)->pipe));
        free((*RT)->infile);
        free((*RT)->outfile);
        (*RT)->infile=NULL;
        (*RT)->outfile=NULL;
        delRunTree (&((*RT)->psubcmd));
        free(*RT);
        *RT=NULL;
    }
}
