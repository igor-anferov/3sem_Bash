/* Модуль предназначен для ввода команды пользователя и разбиения её на лексемы
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"

/* Переопределение системного размера буфера ввода (в символах) */
/*
#undef BUFSIZ
#define BUFSIZ 8
*/

char ___buf[BUFSIZ+1];       /* Объявление буфера ввода (общего для getbuf и
                              ungetbuf) */
int ___free_pointer=BUFSIZ;  /* Указатель первого свободного поля в массиве buf */

/* Функция аналогична getchar, но работает с stdin через буфер buf */
int getbuf(void) {
    if (___free_pointer==BUFSIZ) {
        if (fgets(___buf, BUFSIZ+1, stdin)==NULL)
            return EOF;
        ___free_pointer=-1;
        return getbuf();
    }
    if (___buf[++___free_pointer]==0) {
        ___free_pointer=BUFSIZ;
        return getbuf();
    }
    return ___buf[___free_pointer];
}

/* Функция аналогична ungetch, но работает с буфером buf */
void ungetbuf(int c) {
    ___buf[___free_pointer--]=c;
}

/* Функция добавляет в динамический массив "*p" размера "mem" символ "c" в
   позицию "fp", "fp" при этом увеличивается на 1, при необходимости изменяются
   "p" и "mem" */
void addchar(char **p, int *mem, int *fp, char c) {
    if (*fp>=*mem) {
        *mem=(*mem==0)?2:*mem*2;
        *p=realloc(*p, *mem*sizeof(c));
    }
    (*p)[(*fp)++]=c;
}

/* Функция пропускает всё до первого '\n' включительно, работает с буфером buf
   */
void cleanbuf(void) {
    int c;
    while ((c=getbuf())!='\n' && c!=EOF);
}

/* Функция получает из потока ввода (посредством getbuf) очередное слово
   (простое или специальное) и присваивает переменной "p" ссылку на это слово;
   в случае, если слово введено некорректно, возвращает -1 */
int getword(char **p) {
    int mem=0, fp=0, c;
    *p=NULL;
    while ((c=getbuf())==' ' || c=='\t');
    if (c=='\n')
        return 0;
    if (c==EOF)
        return 1;
    switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
            
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            
        case '_': case '/': case '$': case '-': case '+': case '*': case '.': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto SI;
        }
            
        case '|': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto AO;
        }
        case '&': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto AA;
        }
        case '>': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto AM;
        }
        case ';': case '<': case '#': case '(': case ')': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto WS;
        }
        default: {
            free (*p);
            *p=NULL;
            return -1;
        }
    }
SI: switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
            
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        
        case '_': case '/': case '$': case '-': case '+': case '*': case '.': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto SI;
        }
        default:
            goto EX;
    }
AO: switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
        
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        
        case '_': case '/': case '$': case '-': case '+': case '*': case '.':
        
        case '&': case '>': case ';': case '<': case '#': case '(': case ')':
        
        case ' ': case '\t': case '\n': case EOF:
        goto EX;
        case '|': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto WS;
        }
        default: {
            free (*p);
            *p=NULL;
            return -1;
        }
    }
AA: switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
        
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        
        case '_': case '/': case '$': case '-': case '+': case '*': case '.':
        
        case '|': case '>': case ';': case '<': case '#': case '(': case ')':
        
        case ' ': case '\t': case '\n': case EOF:
            goto EX;
        case '&': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto WS;
        }
        default: {
            free (*p);
            *p=NULL;
            return -1;
        }
    }
AM: switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
        
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        
        case '_': case '/': case '$': case '-': case '+': case '*': case '.':
        
        case '&': case '|': case ';': case '<': case '#': case '(': case ')':

        case ' ': case '\t': case '\n': case EOF:
            goto EX;
        case '>': {
            addchar(p, &mem, &fp, c);
            c=getbuf();
            goto WS;
        }
        default: {
            free (*p);
            *p=NULL;
            return -1;
        }
    }
WS: switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
        
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        
        case '_': case '/': case '$': case '-': case '+': case '*': case '.':
        
        case '&': case '|': case '>': case ';': case '<': case '#': case '(': case ')':
        
        case ' ': case '\t': case '\n': case EOF:
            goto EX;
        default: {
            free (*p);
            *p=NULL;
            return -1;
        }
    }
    
EX:	ungetbuf(c);
    addchar(p, &mem, &fp, '\0');
    return 0;
}

/* Функция удаляет список "wl" */
void dellist(WordList *wl) {
    if (wl!=NULL) {
        dellist (wl->next);
        free (wl->word);
        free (wl);
    }
}

/* Функция добавляет элемент "s" в конец списка "*wl" */
void appendlist(WordList **wl, char *s) {
    WordList *tmp=(WordList *) malloc(sizeof(WordList));
    WordList *p;
    tmp->next=NULL;
    tmp->word=s;
    if ((*wl)==NULL)
        (*wl)=tmp;
    else {
        p=*wl;
        while ((p->next)!=NULL)
            p=p->next;
        p->next=tmp;
    }
}

/* Функция строит список слов и помещает указатель на него в переменную "*wl";
   возвращает 1, если достигнут конец файла и -1, если строка имеет неверный
   формат (иначе 0) */
int getlist(WordList **wl) {
    char *s;
    int ret;
    *wl=NULL;
    while (!(ret=getword(&s)) && s!=NULL)
        appendlist(wl, s);
    if (ret==-1) {
        cleanbuf();
        dellist (*wl);
    }
    return ret;
}

char *getlex(WordList **wl) {
    char *tmp=NULL;
    WordList *p;
    if (*wl!=NULL) {
        tmp=(*wl)->word;
        p=*wl;
        *wl=(*wl)->next;
        free (p);
    }
    return tmp;
}

void ungetlex(WordList **wl, char *lex) {
    WordList *p;
    p=malloc(sizeof(WordList));
    p->word=lex;
    p->next=*wl;
    *wl=p;
}
