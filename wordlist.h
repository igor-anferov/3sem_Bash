typedef struct WordList {
    char *word;
    struct WordList *next;
} WordList;

int getbuf(void);
void ungetbuf(int c);
void addchar(char **p, int *mem, int *fp, char c);
void cleanbuf(void);
int getword(char **p);
void dellist(WordList *wl);
void appendlist(WordList **wl, char *s);
int getlist(WordList **wl);
char *getlex(WordList **wl);
void ungetlex(WordList **wl, char *lex);
