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
    int     md_record_count;
    long    md_total_income;
    long    md_total_expense;
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


static int readInexDataFromFile(InexDataPtr inex, FILE *fp);
static int writeInexDataIntoFile(InexDataPtr inex, FILE *fp);
static int fileNameValidity(const char *fileName);
static int fileExist(const char *fileName);
static int compareDate(Date d1, Date d2);
static int copyRecord(struct record *dest, struct record *src);
static int metaUpdate(InexDataPtr inex, ListNodePtr node, struct record *rec);
static int printRecord(struct record *rec);
static void printRecordHeader();
static void printRecordFooter();

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
        puts("\tMESSAGE: Invalid FileName!");
        return NULL;
    }

    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension)) {
        puts("\tMESSAGE: File already exists!");
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
    inex->meta.md_counter       = 1;
    inex->meta.md_record_count  = 0;
    inex->meta.md_total_income  = 0;
    inex->meta.md_total_expense = 0;
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
        puts("\tMESSAGE: Invalid FileName!");
        return NULL;
    }

    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension) == 0) {
        puts("\tMESSAGE: File doesn't exist!");
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
        puts("\tMESSAGE: Invalid fileName!");
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
        puts("\tMESSAGE: Invalid FileName!");
        return -1;
    }

    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension) == 0) {
        puts("\tMESSAGE: File doesn't exist!");
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


/*
 * Function to add record into into InEx Data
 * record with latest date should be added on Top (head)
 *
 * It is caller functions responsibility to send valid records
 */
int addRecord(InexDataPtr inex, struct record *rec) 
{
    ListNodePtr node;
    ListNodePtr current;

    if (inex == NULL || rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -2;
    }

    node = calloc(1, sizeof(*node));
    if (node == NULL) {
        logError(ERROR_MEMORY_ALLOC);
        return -1;
    }

    inex->meta.md_record_count++;
    rec->r_id   = inex->meta.md_counter++;
    node->next  = NULL;
    copyRecord(&node->rec, rec);

    if (rec->r_info & 1) {
        inex->meta.md_total_income += rec->r_amount;
    } else {
        inex->meta.md_total_expense += rec->r_amount;
    } 

    current = inex->headNode;

    /* if no exiting records */
    if (inex->headNode == NULL) {
        inex->headNode = node;
        return 0;
    }

    /* if the new record needs to be in top */
    if (compareDate(node->rec.r_date, current->rec.r_date) >= 0) {
        node->next = inex->headNode;
        inex->headNode = node;
        return 0;
    }

    while(current != NULL) {
        /* when tail is reached add the new record in the end */
        if (current->next == NULL) {
            current->next = node;
            return 0;
        }

        /* if new record date is >= current->next record date */
        if (compareDate(node->rec.r_date, current->next->rec.r_date) >= 0) {
            node->next = current->next;
            current->next = node;
            return 0;
        }

        current = current->next;
    }

    return 0;
}


int editRecord(InexDataPtr inex, struct record *rec) 
{
    if (inex == NULL || rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    puts("\tMESSAGE: <under development>!");

    return 1;
}


/*
 * Funtion to delete the record based on the record id 
 */
int deleteRecord(InexDataPtr inex, int record_id)
{
    ListNodePtr current;
    ListNodePtr temp;

    if (inex == NULL || record_id < 0) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    current = inex->headNode;

    /* if no existing records */
    if (current == NULL)
        return 1;

    /* If matching record found in the start */
    if (current->rec.r_id == record_id) {
        metaUpdate(inex, current, NULL);
        inex->headNode = current->next;
        free(current);
        return 0;
    }
    
    while (current != NULL) {
        /* If no matching records found */
        if (current->next == NULL)
            return 1;

        /* If matching record found in the middle or end */
        if (current->next->rec.r_id == record_id) {
            metaUpdate(inex, current->next, NULL);
            temp = current->next;
            current->next = temp->next;
            free(temp);
            return 0;
        }

        current = current->next;
    }

    return 1;
}


int viewRecord(InexDataPtr inex, const char *argument)
{
    ListNodePtr current = inex->headNode;
    int count = 0;

    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    /* if no argument, default to show 15 records */
    if (argument == NULL) {
        puts("\tDebug: No argument specified!");
        count = 15;
    } else {
        if (strcmp(argument,"all") == 0) {
            count = -1;
        } else {
            if (sscanf(argument,"%d",&count) <= 0 || count < 0) {
                puts("\tMESSAGE: Invalid command arguments!");
                return -1;
            } 
        }
    }

    printRecordHeader();

    while(current != NULL && (count != 0)) {
        printRecord(&current->rec);
        current = current->next;
        if (count > 0)
            count--;
    }

    printRecordFooter();

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


int infoInexData(InexDataPtr inex) 
{
    long income, expense, balance_main, balance_deci;

    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    income      = inex->meta.md_total_income;
    expense     = inex->meta.md_total_expense;
    balance_main = (income - expense) / 100;
    balance_deci = (income - expense) % 100;
    balance_deci = (balance_deci < 0) ? balance_deci * (-1) : balance_deci;

    printf("\tFile name     : %s.bin\n"   , inex->meta.md_file_name);
    printf("\tcounter       : %d\n"       , inex->meta.md_counter);
    printf("\tTotal records : %d\n"       , inex->meta.md_record_count);
    printf("\tTotal Income  : %ld.%02ld\n", income / 100, income % 100);
    printf("\tTotal Expense : %ld.%02ld\n", expense / 100, expense % 100);
    printf("\tBalance       : %ld.%02ld\n", balance_main, balance_deci);

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

        current_node = current_node->next;
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


static int compareDate(Date d1, Date d2) 
{
    if (d1.year > d2.year)
        return 1;
    if (d1.year < d2.year)
        return -1;

    if (d1.month > d2.month)
        return 1;
    if (d1.month < d2.month)
        return -1;

    if (d1.day > d2.day)
        return 1;
    if (d1.day < d2.day)
        return -1;

    return 0;
} 


static int copyRecord(struct record *dest, struct record *src) 
{
    if (dest == NULL || src == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    dest->r_id       = src->r_id;
    dest->r_info     = src->r_info;
    dest->r_amount   = src->r_amount;
    dest->r_date     = src->r_date;
    strncpy(dest->r_entity, src->r_entity, ENTITY_LEN);
    strncpy(dest->r_comment, src->r_comment, COMMENT_LEN);

    return 0;
} 


/*
 * Function to update the meta data of the inex data based on delete or edit
 *
 * In case of delete, only Node is present
 * In case of edit, a Node (with existing data) and replacing record values
 */
static int metaUpdate(InexDataPtr inex, ListNodePtr node, struct record *rec)
{
    if (inex == NULL || node == NULL) 
        return -1;

    if (node->rec.r_info & 1) {
        inex->meta.md_total_income -= node->rec.r_amount;
    } else {
        inex->meta.md_total_expense -= node->rec.r_amount;
    }

    if (rec == NULL)
        inex->meta.md_record_count--;

    if (rec != NULL && rec->r_amount > 0) {
        if (rec->r_info & 1) {
            inex->meta.md_total_income += rec->r_amount;
        } else {
            inex->meta.md_total_expense += rec->r_amount;
        }
    }

    return 0;
}


static int printRecord(struct record *rec) 
{
    static char type[4];
    static const char *rec_footer = 
        "\n|-----|"
        "------------|"
        "--------------------|"
        "------------|"
        "---------------------------------|";
    static const char *empty_block = 
        "\n|     | ";

    if (rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    strcpy(type,"");

    if (rec->r_info & 1)
        strcpy(type,"+IN");

    printf("| %3s | %10d   %15ld.%02ld   %04d-%02d-%02d   %31s  "
        , type, rec->r_id, (rec->r_amount / 100), (rec->r_amount % 100)
        , rec->r_date.year, rec->r_date.month, rec->r_date.day
        , rec->r_entity);

    printf("%s",empty_block);
    printf("%s",empty_block);
    printf("COMMENT: %s", rec->r_comment);
    printf("%s",empty_block);
        
    puts(rec_footer);

    return 0;
} 


static void printRecordHeader() 
{
    static const char *record_header =
        "\n\t<-----LIST OF RECORDS----->\n"

        "\n|-----|"                              // 7
        "------------|"                          // 13
        "--------------------|"                  // 21
        "------------|"                          // 13
        "---------------------------------|"     // 34

        "\n| ??? |"
        "         ID |"
        "             AMOUNT |"
        "       DATE |"
        "                          ENTITY |"

        "\n|-----|"
        "------------|"
        "--------------------|"
        "------------|"
        "---------------------------------|";

    puts(record_header);
}


static void printRecordFooter() 
{
    static const char *record_footer =
        "\n\t<-------END OF LIST------->\n";
        
    puts(record_footer);
} 