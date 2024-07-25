/*
 * inexData.c
 * 
 * Everything related to InEx data 
 * 
 *  Created on: 26-Jun-2024
 *      Author: deepaks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/inexData.h"
#include "headers/dataDefinition.h"
#include "headers/customError.h"

#define HEADER_LEN      32
#define FOOTER_LEN      32
#define FILE_NAME_LEN   32

typedef struct listNode* ListNodePtr;

struct metaData {
    char    md_header[HEADER_LEN];
    char    md_file_name[FILE_NAME_LEN];
    int     md_counter;
    char    md_footer[FOOTER_LEN];
};

struct listNode {
    struct record       rec;
    struct listNode     *next;
};

struct inexData {
    struct metaData meta;
    ListNodePtr     headNode;
};


static int fileNameValidity(const char *fileName);
static int fileExist(const char *fileName);
static int readInexDataFromFile(InexDataPtr inex, FILE *fp);
static int writeInexDataIntoFile(InexDataPtr inex, FILE *fp);

static const char *header_name = "inex-file-header";
static const char *footer_name = "inex-file-footer";


InexDataPtr createInexData(const char *fileName) 
{
    InexDataPtr inex;
    char fileNameExtension[FILE_NAME_LEN];

    if (fileName == NULL) {
        logError(ERROR_ARGUMENT);
        return NULL;
    }
        
    if (fileNameValidity(fileName) == 0) {
        printf("\tMESSAGE: Invalid FileName!\n");
        return NULL;
    }

    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension)) {
        printf("\tMESSAGE: File already exists!\n");
        return NULL;
    }

    inex = calloc(1, sizeof(*inex));
    if (inex == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        return inex;
    }

    strncpy(inex->meta.md_header, header_name, HEADER_LEN);
    strncpy(inex->meta.md_footer, footer_name, FOOTER_LEN);
    strncpy(inex->meta.md_file_name, fileName, FILE_NAME_LEN);
    inex->meta.md_counter = 1;
    inex->headNode = NULL;

    return inex;
}


InexDataPtr openInexDataFromFile(const char *fileName) 
{
    FILE *fp;
    InexDataPtr inex;
    char fileNameExtension[FILE_NAME_LEN];

    if (fileName == NULL) {
        logError(ERROR_ARGUMENT);
        return NULL;
    }

    if (fileNameValidity(fileName) == 0) {
        printf("\tMESSAGE: Invalid FileName!\n");
        return NULL;
    }

    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension) == 0) {
        printf("\tMESSAGE: File doesn't exist!\n");
        return NULL;
    }

    fp = fopen(fileNameExtension,"r");

    if (fp == NULL) {
        logError(ERROR_FILE_OPEN);
        return NULL;
    }

    inex = calloc(1, sizeof(*inex));
    if (inex == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        goto end_open;
    }

    if (readInexDataFromFile(inex, fp) != 0) {
        destroyInexData(inex);
        inex = NULL;
    }

end_open:
    fclose(fp);

    return inex;
}


int saveInexData(InexDataPtr inex) 
{
    FILE *fp;
    char fileNameExtension[FILE_NAME_LEN];
    int returnCode;

    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (fileNameValidity(inex->meta.md_file_name) == 0) {
        printf("\tMESSAGE: Invalid fileName!\n");
        return -1;
    }

    strncpy(fileNameExtension, inex->meta.md_file_name, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    fp = fopen(fileNameExtension, "w");
    if (fp == NULL) {
        logError(ERROR_FILE_OPEN);
        return -1;
    }

    returnCode = writeInexDataIntoFile(inex, fp);
    if (returnCode < 0) {
        logError(ERROR_FILE_WRITE);
        fclose(fp);
        removeInexFile(inex->meta.md_file_name);
        return returnCode;
    }

    fclose(fp);

    return returnCode;
}


void destroyInexData(InexDataPtr inex) 
{
    ListNodePtr current_node;
    ListNodePtr previous_node;

    if (inex == NULL)
        return;

    if (inex->headNode != NULL) 
    {
        current_node = inex->headNode;

        while(current_node != NULL) 
        {
            previous_node = current_node;
            current_node = current_node->next;
            free(previous_node);
        }
    }

    free(inex);
}


int removeInexFile(const char *fileName) 
{
    char fileNameExtension[FILE_NAME_LEN];

    if (fileName == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }
        
    if (fileNameValidity(fileName) == 0) {
        printf("\tMESSAGE: Invalid FileName!\n");
        return -1;
    }

    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension) == 0) {
        printf("\tMESSAGE: File doesn't exist!\n");
        return -1;
    }

    /*
        Future implementation: Check if it is really an InEx bin file
    */

    if (remove(fileNameExtension) != 0) {
        logError(ERROR_FILE_REMOVE);
        return -1;
    }

    return 0;
}


void listInexFile() 
{
    puts("\tMESSAGE: <under development>!");
}


int addRecord(InexDataPtr inex, struct record *rec) 
{
    if (inex == NULL || rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    puts("\tMESSAGE: <under development>!");

    return 0;
}


int editRecord(InexDataPtr inex, struct record *rec) 
{
    if (inex == NULL || rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    puts("\tMESSAGE: <under development>!");

    return 0;
}


int deleteRecord(InexDataPtr inex, int record_id)
{
    if (inex == NULL || record_id < 0) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    puts("\tMESSAGE: <under development>!");

    return 0;
}


int viewRecord(InexDataPtr inex, const char *argument)
{
    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (argument == NULL) {
        puts("\tDebug: No argument specified!");
    }

    puts("\tMESSAGE: <under development>!");

    return 0;
}


int filterRecord(InexDataPtr inex, char **token)
{
    if (inex == NULL || token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    puts("\tMESSAGE: <under development>!");

    return 0;
}


int infoInexData(InexDataPtr inex) {
    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    puts("\tMESSAGE: <under development>!");

    return 0;
}


void showFileName(InexDataPtr inex) 
{
    if (inex == NULL)
        return;

    printf("%s",inex->meta.md_file_name);
}


static int readInexDataFromFile(InexDataPtr inex, FILE *fp) 
{
    struct record temp_record;
    ListNodePtr temp_node;

    if (inex == NULL || fp == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (fread(&inex->meta, sizeof(inex->meta), 1, fp) != 1) {
        logError(ERROR_FILE_READ);
        return -1;
    }
    
    while(fread(&temp_record, sizeof(temp_record), 1, fp) == 1) {
        if (inex->headNode == NULL) {
            temp_node = (ListNodePtr) calloc(1, sizeof(*temp_node));

            if (temp_node == NULL) {
                logError(ERROR_MEMORY_ALLOC);
                return -1;
            }

            inex->headNode = temp_node;
        } else {
            temp_node->next = (ListNodePtr) calloc(1, sizeof(*temp_node));

            if (temp_node->next == NULL) {
                logError(ERROR_MEMORY_ALLOC);
                return -1;
            } 

            temp_node = temp_node->next;
        }

        temp_node->rec  = temp_record;
        temp_node->next = NULL;
    }

    return 0;
}


static int writeInexDataIntoFile(InexDataPtr inex, FILE *fp) 
{
    ListNodePtr current_node;

    if (inex == NULL || fp == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (fwrite(&inex->meta, sizeof(inex->meta), 1, fp) != 1) {
        logError(ERROR_FILE_WRITE);
        return -1;
    }

    current_node = inex->headNode;

    while (current_node != NULL) {
        if (fwrite(&current_node->rec, sizeof(current_node->rec), 1, fp) != 1) {
            logError(ERROR_FILE_WRITE);
            return -1;
        }
    }

    return 0;
}


static int fileNameValidity(const char *fileName) 
{
    int index; 
    char ch;

    if (fileName == NULL) {
        logError(ERROR_ARGUMENT);
        return 0;
    }

    for (index = 0; index < FILE_NAME_LEN; index++) {
        ch = fileName[index];

        if (ch == '\0')
            break;

        if (index >= (FILE_NAME_LEN - 7))
            return 0;

        if ((ch >= '0' && ch <= '9') 
                || (ch >= 'a' && ch <= 'z') 
                || (ch >= 'A' && ch <= 'Z') 
                || (ch == '-') 
                || (ch == '_')) {
            continue;
        }

        return 0;
    }

    return 1;
}


static int fileExist(const char *fileName) 
{
    FILE *fp;

    if (fileName == NULL) {
        logError(ERROR_ARGUMENT);
        return 0;
    }

    fp = fopen(fileName, "r");

    if (fp == NULL)
        return 0;

    fclose(fp);

    return 1;
} 