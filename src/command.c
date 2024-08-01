/*
 * command.c
 * 
 * Everything related to Commands and AppData (Excluding InEx Data)
 * 
 *  Created on: 30-Jun-2024
 *      Author: deepaks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/command.h"
#include "headers/consoleInput.h"
#include "headers/customError.h"
#include "headers/inexData.h"
#include "headers/recordFunction.h"
#include "headers/appInfo.h"

#define CMD_LEN         256
#define MAX_TOKEN       10 

/* Function pointer definition for wrapper functions */
typedef int (*CommandFunction)(AppDataPtr appData);

/* Lookup template for command and its function */
typedef struct {
    const char      *command;
    CommandFunction cmdFunction;
} CommandLookup;

/* completion of definition for incomplete dataType AppDataPtr */
struct appData {
    InexDataPtr     inex;
    char            *cmd;
    char            **token;
    int             saved;
};


/* static wrapper functions */
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

/* other static functions */
static int getCommand(AppDataPtr appData);
static int setToken(AppDataPtr appData);
static int isSpace(char ch);
static int printCommandPrompt(AppDataPtr appData);
static int saveConfirmation(void);
static int validTokenCount(AppDataPtr appData, int min, int max);
static int no_of_token(AppDataPtr appData);
//static void showToken(AppDataPtr appData);


/* Declaring static Lookup table */
static const CommandLookup cmd_lookup[] = {
    {"quit"     , quit_wrapper      },
    {"help"     , help_wrapper      },
    {"about"    , about_wrapper     },
    {"create"   , create_wrapper    },
    {"open"     , open_wrapper      },
    {"remove"   , remove_wrapper    },
    {"list"     , list_wrapper      },
    {"add"      , add_wrapper       },
    {"edit"     , edit_wrapper      },
    {"delete"   , delete_wrapper    },
    {"view"     , view_wrapper      },
    {"filter"   , filter_wrapper    },
    {"info"     , info_wrapper      },
    {"save"     , save_wrapper      },
    {"close"    , close_wrapper     },
    {NULL       , NULL              }
};


/*
 * To create and handle for appData
 */
AppDataPtr createAppData(void) 
{
    AppDataPtr appData = calloc(1, sizeof(*appData));
    if (appData == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        return NULL;
    }

    appData->cmd = calloc(CMD_LEN, sizeof(*(appData->cmd)));
    if (appData->cmd == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        destroyAppData(appData);
        return NULL;
    }

    appData->token = calloc(MAX_TOKEN, sizeof(*(appData->token)));
    if (appData->token == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        destroyAppData(appData);
        return NULL;
    }

    appData->inex       = NULL;
    appData->saved      = 0;

    return appData;
}


/*
 * Function to perform the command which will be entered by the user
 */
int performGetCommand(AppDataPtr appData) 
{
    int index = 0;
    int returnCode;
    
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_FUNC_ARG);
        return -1;
    }

    if ((returnCode = getCommand(appData)) != 0)
        return returnCode;

    if (appData->token[0] == NULL) {
        logError(ERROR_WENT_WRONG);
        return -2;
    } 
    
    while (cmd_lookup[index].command != NULL) {
        if (strcmp(cmd_lookup[index].command, appData->token[0]) == 0) {
            if (cmd_lookup[index].cmdFunction(appData) != 0) {
                return -1;
            }
            return index;
        }
        index++;
    }

    generic_wrapper(appData);

    return index;
} 


/*
 * to destroy and free AppData 
 */
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


/*
 * A Generic warpper function to handle the invalid command operation 
 */
static int generic_wrapper(AppDataPtr appData) 
{
    puts("\tMESSAGE: unsupported command!");

    return 0;
}


static int quit_wrapper(AppDataPtr appData) 
{
    if (appData->inex != NULL) {
        return close_wrapper(appData);
    }

    return 0;
}


static int help_wrapper(AppDataPtr appData) 
{
    if (validTokenCount(appData, 1, 2) == 0)
        return 1;

    help(appData->token);

    return 0;
}


static int about_wrapper(AppDataPtr appData) 
{
    about();

    return 0;
}


static int create_wrapper(AppDataPtr appData) 
{
    if (appData->inex != NULL) {
        puts("\tMESSAGE: CLOSE the file first!");
        return 2;
    }

    if (validTokenCount(appData, 2, 2) == 0)
        return 3;

    appData->inex   = createInexData(appData->token[1]);

    if (appData->inex != NULL) {
        appData->saved  = 0;
        return 0;
    }
    
    return 1;
}


static int open_wrapper(AppDataPtr appData) 
{
    if (appData->inex != NULL) {
        puts("\tMESSAGE: CLOSE the file first!");
        return 2;
    }

    if (validTokenCount(appData, 2, 2) == 0)
        return 3;

    appData->inex   = openInexDataFromFile(appData->token[1]);

    if (appData->inex == NULL) {
        return 1;
    }

    appData->saved  = 1;

    return 0;
}


static int remove_wrapper(AppDataPtr appData) 
{
    if (appData->inex != NULL) {
        puts("\tMESSAGE: CLOSE the file first!");
        return 2;
    }

    if (validTokenCount(appData, 2, 2) == 0)
        return 3;

    if (removeInexFile(appData->token[1]) != 0) {
        return 1;
    }

    return 0;
}


static int list_wrapper(AppDataPtr appData) 
{
    listInexFile();

    return 0;
}


static int add_wrapper(AppDataPtr appData) 
{
    Record rec;
    int returnCode  = 0;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2;
    }

    rec.r_id    = 0;
    rec.r_info  = -1;

    if (validTokenCount(appData, 2, 2) == 0)
        return 3;

    if (strcmp(appData->token[1], "in") == 0)
        rec.r_info = 1;

    if (strcmp(appData->token[1], "ex") == 0)
        rec.r_info = 0;

    if (rec.r_info < 0) {
        puts("\tMESSAGE: Enter valid arguments!");
        return 4;
    }

    /* mandatory field check = 1 (non-zero) indicates YES */
    returnCode = getRecordFromConsole(&rec, 1); 
    if (returnCode < 0) {
        logError(ERROR_STD_INPUT);
        return -1;
    }
    if (returnCode > 0) {
        puts("\tMESSAGE: Enter valid values in Mandatory field!");
        return 5;
    }

    returnCode = addRecord(appData->inex, &rec);
    if (returnCode != 0) {
        puts("\tMESSAGE: No record is added!");
        return 1;
    }

    appData->saved = 0;

    return 0;
}


static int edit_wrapper(AppDataPtr appData) 
{
    Record rec;
    int returnCode  = 0;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2;
    }

    if (validTokenCount(appData, 2, 2) == 0)
        return 3;

    returnCode = sscanf(appData->token[1], "%d", &rec.r_id);
    if (returnCode <= 0 || rec.r_id < 0) {
        puts("\tMESSAGE: Enter valid arguments!");
        return 4;
    }

    /* mandatory field check = 0 (zero) indicates NO */
    returnCode = getRecordFromConsole(&rec, 0);   
    if (returnCode < 0) {
        logError(ERROR_STD_INPUT);
        return -1;
    }
    
    returnCode = editRecord(appData->inex, &rec);
    if (returnCode != 0) {
        puts("\tMESSAGE: No record is edited!");
        return 1;
    }

    appData->saved = 0;

    return 0;
}


static int delete_wrapper(AppDataPtr appData) 
{
    int id, returnCode;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2;
    }

    if (validTokenCount(appData, 2, 2) == 0)
        return 3;

    returnCode = sscanf(appData->token[1], "%d", &id);
    if (returnCode <= 0 || id < 0) {
        puts("\tMESSAGE: Enter valid arguments!");
        return 4;
    }

    returnCode = deleteRecord(appData->inex, id);
    if (returnCode != 0) {
        puts("\tMESSAGE: No record is deleted!");
        return 1;
    }

    appData->saved = 0;

    return 0;
}


static int view_wrapper(AppDataPtr appData) 
{
    int returnCode;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2;
    }

    if (validTokenCount(appData, 1, 2) == 0)
        return 3;

    returnCode = viewRecord(appData->inex, appData->token[1]);
    if (returnCode != 0)
        return 1;

    return 0;
}


static int filter_wrapper(AppDataPtr appData) 
{
    int returnCode;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2;
    }

    if (validTokenCount(appData, 4, 5) == 0)
        return 3;

    returnCode = filterRecord(appData->inex, appData->token);
    if (returnCode != 0) {
        puts("\tMESSAGE: Enter valid arguments!");
        return 1;
    }

    return 0;
}


static int info_wrapper(AppDataPtr appData) 
{
    int returnCode;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2;
    }

    puts("");

    returnCode = infoInexData(appData->inex);
    if (returnCode != 0) 
        return 1;

    printf("\tstatus        : ");
    if (appData->saved) {
        puts("saved");
    } else {
        puts("*not saved");
    }

    puts("");

    return 0;
} 


static int save_wrapper(AppDataPtr appData) 
{
    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 2; 
    }
        
    if (appData->saved) {
        puts("\tMESSAGE: Already saved!");
        return 3;
    }

    if (saveInexData(appData->inex) != 0) {
        return 1;
    }

    appData->saved  = 1;

    return 0;
}


static int close_wrapper(AppDataPtr appData) 
{
    int returnCode;

    if (appData->inex == NULL) {
        puts("\tMESSAGE: No File opened!");
        return 0;
    }
        
    /* if file not saved, ask confirmation to save */
    if (appData->saved == 0) {
        while((returnCode = saveConfirmation()) == 0);

        if (returnCode < 0) {
            logError(ERROR_STD_INPUT);
            return -1;
        }

        /* indicates 'cancel', do not save or close */
        if (returnCode == 3)
            return 1;

        /* indicates 'yes', try to save before close */
        if (returnCode == 1) 
            save_wrapper(appData);
    }

    destroyInexData(appData->inex);
    appData->inex = NULL;
    appData->saved = 0;

    return 0;
} 


/*
 * Function to get command from the user
 */
static int getCommand(AppDataPtr appData) 
{
    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_FUNC_ARG);
        return -2;
    }
        
    memset(appData->cmd, 0, CMD_LEN * sizeof(*(appData->cmd)));

    if (printCommandPrompt(appData) != 0)
        return -3;
    
    if (getStringFromConsole(appData->cmd, CMD_LEN) < 0) {
        logError(ERROR_STD_INPUT);
        return -1;
    }

    if (setToken(appData) <= 0) 
        return 1;

    return 0;
}


/*
 * Function to process the command into individual tokens
 */
static int setToken(AppDataPtr appData) 
{
    int cmd_index       = 0;
    int tok_index       = 0;
    int insideQuote     = 0;
    int enable_token    = 1;
    char ch;

    memset(appData->token, 0, MAX_TOKEN * sizeof(*(appData->token)));

    for ( ; cmd_index < CMD_LEN; cmd_index++) {
        ch = appData->cmd[cmd_index];

        /* only limited amount of token should be processed */
        if (tok_index >= (MAX_TOKEN - 1) && enable_token) 
            break; 

        /* stop processing when command reached the end */
        if (ch == '\n' || ch == '\0' || ch == EOF) {
            appData->cmd[cmd_index] = '\0';

            /* If empty command, first token should be empty too */
            if (tok_index == 0) 
                appData->token[tok_index] = &appData->cmd[cmd_index];

            break;
        }

        /* when the token seperator comes (i.e., space outside token) */
        if (isSpace(ch)) {
            if (!(insideQuote)) {
                appData->cmd[cmd_index] = '\0';
                enable_token = 1;
            }

            continue;
        }

        /* track if we are inside or outside the token */
        if (appData->cmd[cmd_index] == '\'') 
            insideQuote = !(insideQuote);

        /* if token is enabled, should point to current index of cmd */
        if (enable_token) {
            appData->token[tok_index] = &appData->cmd[cmd_index];
            tok_index++;
            enable_token = 0;
        }
    }

    if (insideQuote) {
        puts("\tMESSAGE: close quote missing!");
        return 0;
    }

    return tok_index;
}


/*
 * Function to check if it is <blank> space 
 */
static int isSpace(char ch) 
{
    if (ch == ' ' || ch == '\t')
        return 1;

    return 0;
}


/*
 * Function to print the default prompt
 */
static int printCommandPrompt(AppDataPtr appData) 
{
    static const char *cmd_line = ">> ";

    if (appData == NULL || appData->cmd == NULL || appData->token == NULL) {
        logError(ERROR_FUNC_ARG);
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
    fflush(stdout);

    return 0;
}


/*
 * Function for save confirmation logic
 */
static int saveConfirmation(void) 
{
    static char buffer[2]; 
    static const char *message = "\tDo you want to save? [y/n/c] ";

    memset(buffer, 0, 2);

    printf("%s", message);
    
    if (getStringFromConsole(buffer, 2) < 0)
        return -1;

    if (buffer[0] == 'y' || buffer[0] == 'Y')
        return 1;

    if (buffer[0] == 'n' || buffer[0] == 'N')
        return 2;

    if (buffer[0] == 'c' || buffer[0] == 'C')
        return 3;

    return 0;
}


/*
 * To validate total no of tokens entered 
 */
static int validTokenCount(AppDataPtr appData, int min, int max) 
{
    int count;

    if (appData == NULL || appData->cmd == NULL || appData->token == NULL)
        return 0;

    count = no_of_token(appData);

    if (count < min) {
        puts("\tMESSAGE: Missing Command Arguments!");
        return 0;
    }

    if (count > max) {
        puts("\tMESSAGE: Too many Command Arguments!");
        return 0;
    }

    return 1;
}


/*
 * To return the no of tokens present 
 */
static int no_of_token(AppDataPtr appData) 
{
    int index = 0;

    if (appData == NULL || appData->cmd == NULL || appData->token == NULL)
        return -1;

    while (appData->token[index] != NULL && index < MAX_TOKEN) 
        index++;

    return index;
}


/*
// only for debugging
static void showToken(AppDataPtr appData) 
{
    int index = 0;

    puts("");
    while(appData->token[index] != NULL) {
        printf("%d - %s\n", index, appData->token[index]);
        index++;
    }
} 
*/ 