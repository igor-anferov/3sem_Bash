typedef struct RunTree {
    char **argv;       /* список из имени команды и аргументов */
    char *infile;      /* переназначенный файл стандартного ввода */
    char *outfile;     /* переназначенный файл стандартного вывода */
    int append;        /* ?перенаправенный выход работает в режиме append? */
    int backgrnd;      /* ?команда подлежит выполнению в фоновом режиме? */
    int logical;       /* 0 - безусловное выполнение следующей команды (;)
                        1 - выполнение в случае успешного завершения (&&)
                        2 - выполнение в случае неуспешного завершения (||) */
    struct RunTree* psubcmd;  /* команды для запуска в дочернем shell */
    struct RunTree* pipe;     /* следующая команда после “|” */
    struct RunTree* next;     /* следующая после “;” (или после “&”) */
} RunTree;

int getShellCommand (RunTree**, WordList**);
int getCommandList (RunTree**, WordList**);
int getPipeline (RunTree**, WordList**);
int getCommand (RunTree**, WordList**);
int getSimpleCommand (RunTree**, WordList**);
int getIO (RunTree**, WordList**);
void getArguments (RunTree**, WordList**);
void delRunTree (RunTree**);
