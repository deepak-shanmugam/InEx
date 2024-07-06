#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/appInterface.h"
#include "headers/inexData.h"
#include "headers/functionStrategy.h"
#include "headers/customError.h"
#include "headers/appInfo.h"

#define CMD_LEN     512
#define MAX_TOKEN   10 

#define SAVE_INPUT_LEN  16

struct appData {
    InexDataPtr     inex;
    char            *cmd;
    char            **token;
    int             saved;
};

static int getCommand(AppDataPtr appData);
static int setToken(AppDataPtr appData);
static int isSpace(char ch);
static int saveConfirmation(void);
static int printCommandPrompt(AppDataPtr appData);
//static void showToken(AppDataPtr appData);

/* wrapper functions */
static int generic_wrapper(AppDataPtr appData);
static int quit_wrapper(AppDataPtr appData);
static int help_wrapper(AppDataPtr appData);
static int about_wrapper(AppDataPtr appData);

static int create_wrapper(AppDataPtr appData);
static int open_wrapper(AppDataPtr appData);
static int remove_wrapper(AppDataPtr appData);
static int list_wrapper(AppDataPtr appData);

static int add_wrapper(AppDataPtr appData);
static int edit_wrapper(AppDataPtr appData);
static int delete_wrapper(AppDataPtr appData);
static int view_wrapper(AppDataPtr appData);
static int filter_wrapper(AppDataPtr appData);
static int info_wrapper(AppDataPtr appData);

static int save_wrapper(AppDataPtr appData);
static int close_wrapper(AppDataPtr appData);


static const CommandLookup cmd_lookup[] = {
    {"quit"     , quit_wrapper      , 1},
    {"help"     , help_wrapper      , 1},
    {"about"    , about_wrapper     , 1},
    {"create"   , create_wrapper    , 2},
    {"open"     , open_wrapper      , 2},
    {"remove"   , remove_wrapper    , 2},
    {"list"     , list_wrapper      , 1},
    {"add"      , add_wrapper       , 2},
    {"edit"     , edit_wrapper      , 2},
    {"delete"   , delete_wrapper    , 2},
    {"view"     , view_wrapper      , 1},
    {"filter"   , filter_wrapper    , 4},
    {"info"     , info_wrapper      , 1},
    {"save"     , save_wrapper      , 1},
    {"close"    , close_wrapper     , 1},
    {NULL       , NULL}
};


AppDataPtr createAppData(void) 
{
    AppDataPtr appData = (AppDataPtr) calloc(1, sizeof(*appData));
    if (appData == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        return NULL;
    }

    appData->cmd = (char *) calloc(CMD_LEN, sizeof(*(appData->cmd)));
    if (appData->cmd == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        destroyAppData(appData);
        return NULL;
    }

    appData->token = (char **) calloc(MAX_TOKEN, sizeof(*(appData->token)));
    if (appData->token == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        destroyAppData(appData);
        return NULL;
    }

    appData->inex       = NULL;
    appData->saved      = 0;

    return appData;
}


int performGetCommand(AppDataPtr appData) 
{
    int index;
    int returnCode;
    
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    returnCode = getCommand(appData);
    if (returnCode < 0) 
        return returnCode;

    returnCode = setToken(appData);
    /* If error occurred */
    if (returnCode < 0) 
        return returnCode;
    /* if the number of eligible tokens is zero */
    if (returnCode == 0)    
        return 1;

    // showToken(appData); 

    index = 0;
    while (cmd_lookup[index].command != NULL) {
        if (strcmp(cmd_lookup[index].command, appData->token[0]) == 0) {
            if (returnCode < cmd_lookup[index].min_token) {
                printf("\tMESSAGE: missing arguments!\n");
                return index;
            }

            if (cmd_lookup[index].cmdFunction(appData) < 0)
                return -1;

            return index;
        }

        index++;
    }

    generic_wrapper(appData);

    return index;
}


void destroyAppData(AppDataPtr appData) 
{
    if (appData == NULL)
        return;

    if (appData->inex != NULL) {
        destroyInexData(appData->inex);
        appData->inex = NULL;
    }

    if (appData->cmd != NULL) {
        free(appData->cmd);
        appData->cmd = NULL;
    }

    if (appData->token != NULL) {
        free(appData->token);
        appData->token = NULL;
    }
        
    free(appData);
}


void createTemporaryBackup(AppDataPtr appData) 
{
    if (appData == NULL) {
        logError(ERROR_ARGUMENT);
        return;
    }

    if (appData->inex != NULL && appData->saved == 0) {
        printf("\n\tTemporary backup: <under development>!\n");
    }
}


static int getCommand(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }
        
    memset(appData->cmd, 0, CMD_LEN * sizeof(*(appData->cmd)));

    if (printCommandPrompt(appData) != 0)
        return -1;
    
    if (fgets(appData->cmd, CMD_LEN, stdin) == NULL) {
        logError(ERROR_INPUT);
        return -1;
    }

    return 0;
}


static int setToken(AppDataPtr appData) 
{
    int cmd_index;
    int tok_index;
    int insideQuote;
    int enable_token;
    char ch;

    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    cmd_index       = 0;
    tok_index       = 0;
    insideQuote     = 0;
    enable_token    = 1;

    memset(appData->token, 0, MAX_TOKEN * sizeof(*(appData->token)));

    for ( ; cmd_index < CMD_LEN; cmd_index++) {
        ch = appData->cmd[cmd_index];

        if (ch == '\n' || ch == '\0' || ch == EOF) {
            appData->cmd[cmd_index] = '\0';

            if (tok_index == 0) 
                appData->token[tok_index] = &appData->cmd[cmd_index];

            break;
        }

        if (isSpace(ch)) {
            if (!(insideQuote)) {
                appData->cmd[cmd_index] = '\0';
                enable_token = 1;
            }

            continue;
        }

        if (tok_index >= (MAX_TOKEN - 1)) 
            break; 

        if (appData->cmd[cmd_index] == '\'') 
            insideQuote = !(insideQuote);

        if (enable_token) {
            appData->token[tok_index] = &appData->cmd[cmd_index];
            tok_index++;
            enable_token = 0;
        }
    }

    if (insideQuote) {
        printf("\tMESSAGE: missing close quote\n");
        return 0;
    }

    return tok_index;
}


static int isSpace(char ch) 
{
    if (ch == ' ' || ch == '\t')
        return 1;

    return 0;
}


static int printCommandPrompt(AppDataPtr appData) 
{
    static const char *cmd_line = ">> ";

    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (appData->inex != NULL) {
        printf("%s", cmd_line);

        if (appData->saved == 0)
            printf("*");

        showFileName(appData->inex);
        printf(" ");
    }

    printf("%s", cmd_line);

    return 0;
}


static int saveConfirmation(void) 
{
    char ch;
    static char buffer[SAVE_INPUT_LEN]; 
    static const char *message = "\n\tdo you want to save before close? [y/n/c] ";

    memset(buffer, 0, SAVE_INPUT_LEN);

    printf("%s", message);
    
    if (fgets(buffer, SAVE_INPUT_LEN, stdin) == NULL)
        return -1;

    ch = buffer[0];

    if (ch == 'y' || ch == 'Y')
        return 1;

    if (ch == 'n' || ch == 'N')
        return 2;

    if (ch == 'c' || ch == 'C')
        return 3;

    return 0;
}


/*
static void showToken(AppDataPtr appData) {
    int index = 0;

    while(appData->token[index] != NULL) {
        printf("%d - %s\n", index, appData->token[index]);
        index++;
    }
}
*/


static int generic_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (appData->token[0] == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\tMESSAGE: unsupported command!\n");

    return 0;
}


static int quit_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }
        
    if (appData->inex != NULL)
        close_wrapper(appData);

    return 0;
}


static int help_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    help(appData->token);

    return 0;
}


static int about_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    about();

    return 0;
}


static int create_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->token == NULL || appData->token[1] == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }
    
    if (appData->inex != NULL) {
        printf("\tMESSAGE: CLOSE the file first!\n");
        return 1;
    }

    appData->inex   = createInexData(appData->token[1]);
    appData->saved  = 0;

    return 0;
}


static int open_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->token == NULL || appData->token[1] == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (appData->inex != NULL) {
        printf("\tMESSAGE: CLOSE the file first!\n");
        return 1;
    }

    appData->inex   = openInexDataFromFile(appData->token[1]);
    appData->saved  = 1;

    return 0;
}


static int remove_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (appData->inex != NULL) {
        printf("\tMESSAGE: CLOSE the file first!\n");
        return 1;
    }

    removeInexFile(appData->token[1]);

    return 0;
}


static int list_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
}


static int add_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
}


static int edit_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
}


static int delete_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
}


static int view_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
}


static int filter_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
}


static int info_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\n\tMessage: <under development>!\n");

    return 0;
} 


static int save_wrapper(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (appData->inex == NULL) {
        printf("\tMESSAGE: No file opened!\n");
        return 0; 
    }
        
    if (appData->saved) {
        printf("\tMESSAGE: Already saved!\n");
        return 0;
    }

    if (saveInexData(appData->inex) == 0) 
        appData->saved  = 1;

    return 0;
}


static int close_wrapper(AppDataPtr appData) 
{
    int confirmation = 0;

    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (appData->inex == NULL) {
        printf("\tMESSAGE: No File opened!\n");
        return 0;
    }
        
    if (appData->saved == 0) {
        while((confirmation = saveConfirmation()) == 0);

        if (confirmation < 0) {
            logError(ERROR_INPUT);
            return -1;
        }

        if (confirmation == 1) 
            save_wrapper(appData);
    }

    if (confirmation != 3) {
        destroyInexData(appData->inex);
        appData->inex = NULL;
        appData->saved = 0;
    }

    return 0;
} 