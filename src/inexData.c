/*
 * inexData.c
 * 
 * Everything related to InEx data operations
 * 
 *  Created on: 26-Jun-2024
 *      Author: deepaks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/inexData.h"
#include "headers/customError.h"
#include "headers/dataDefinition.h"
#include "headers/dataFunction.h"

#define HEADER_LEN      32
#define FOOTER_LEN      32
#define FILE_NAME_LEN   32

/* Incomplete Datatype */
typedef struct listNode* ListNodePtr;

/* Function Pointer type definition for filter functions */
typedef int (*FilterFunction)(InexDataPtr inex, char **token);

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

/* Lookup template for filter based on fieldName */
typedef struct {
    const char      *fieldName;
    FilterFunction  filter;
} FilterLookup;


static int readInexDataFromFile(InexDataPtr inex, FILE *fp);
static int writeInexDataIntoFile(InexDataPtr inex, FILE *fp);
static int fileNameValidity(const char *fileName);
static int fileExist(const char *fileName);
static int metaUpdate(InexDataPtr inex, ListNodePtr node, struct record *rec);
static void printCalculation(int no_of_rec, long income, long expense);

/* filter related functions */
static int filterByDate(InexDataPtr inex, char **token);
static int filterByAmount(InexDataPtr inex, char **token);
static int isRecordUnderDateRange(struct record *rec,Date *d1,Date *d2);
static int isRecordUnderAmountRange(struct record *rec,long *a1, long *a2);

/* temporary functions to print records in terminal */
static int printRecord(struct record *rec);
static void printRecordHeader();
static void printRecordFooter();
static void printComment(const char *comment);

static const char *header_name = "inex-file-header";
static const char *footer_name = "inex-file-footer";

/* Declaring static Lookup table for filter */
static const FilterLookup filter_lookup[] = {
    {"date"     , filterByDate},
    {"amount"   , filterByAmount},
    {NULL, NULL}  
};


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

    /* add file extension (.bin) to check if file already exist */
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

    /* Initialize necessary field values for newly created inex data */
    strncpy(inex->meta.md_header, header_name, HEADER_LEN);
    strncpy(inex->meta.md_footer, footer_name, FOOTER_LEN);
    strncpy(inex->meta.md_file_name, fileName, FILE_NAME_LEN);
    inex->meta.md_counter       = 1;
    inex->meta.md_record_count  = 0;
    inex->meta.md_total_income  = 0;
    inex->meta.md_total_expense = 0;
    inex->headNode              = NULL;

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

    /* add file extension (.bin) to check if file already exist */
    strncpy(fileNameExtension, fileName, FILE_NAME_LEN);
    strncat(fileNameExtension, ".bin", 5);

    if (fileExist(fileNameExtension) == 0) {
        puts("\tMESSAGE: File doesn't exist!");
        return NULL;
    }

    fp = fopen(fileNameExtension,"rb");

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


/*
 * Function to show all the meta data of the InEx file
 */
int infoInexData(InexDataPtr inex) 
{
    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    printf("\tFile name     : %s.bin\n"   , inex->meta.md_file_name);
    printf("\tcounter       : %d\n"       , inex->meta.md_counter);
    printCalculation(inex->meta.md_record_count
        , inex->meta.md_total_income, inex->meta.md_total_expense);
    
    return 0;
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

    fp = fopen(fileNameExtension, "wb");
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
    /*
     * Future implementation:
     *  To list all the InEx file present in Current Directory 
     */
}


/*
 * Function to add record into into InEx Data
 * record with latest date should be added on Top (head)
 *
 * It is caller functions responsibility to send valid records
 * Note: Currently only allowing upto 99,999 records only
 */
int addRecord(InexDataPtr inex, struct record *rec) 
{
    ListNodePtr node;
    ListNodePtr current;

    if (inex == NULL || rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -2;
    }

    /*
     * allow only 99,999 records (including deleted records)
     * Future changes might be happen
     *
     * max_amount   : 999,999,999,999 
     * max_amount_with_decimal_digit in storage:
     *              : 99,999,999,999,999
     * max_records  : 1,00,000 - 1 (99,999)
     *
     * max_amount (including decimal) x max_records
     *              : 9,999,999,999,999,900,000
     *
     * max_long_value: (signed)
     *              : 9,223,372,036,854,775,808 
     *
     * The above reason for restriction 
     */
    if (inex->meta.md_counter > 99999) {
        puts("\tMESSAGE: Max record limit reached!");
        return 1;
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
    ListNodePtr current;
    int year, month, day;

    if (inex == NULL || rec == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    /* Existing ID are always lesser than current counter */
    if (rec->r_id >= inex->meta.md_counter) 
        return 1;

    current = inex->headNode;

    while(current != NULL) {
        if (current->rec.r_id == rec->r_id) {
            year    = rec->r_date.year;
            month   = rec->r_date.month;
            day     = rec->r_date.day;

            /* meta data updation */
            metaUpdate(inex, current, rec);
            
            /* edit only eligible records */
            if (rec->r_amount >= 0)
                current->rec.r_amount = rec->r_amount;

            if (isValidDate(year, month, day))
                current->rec.r_date = rec->r_date;

            if (strcmp(rec->r_entity, "") != 0)
                strncpy(current->rec.r_entity, rec->r_entity, ENTITY_LEN);

            if (strcmp(rec->r_comment, "") != 0)
                strncpy(current->rec.r_comment, rec->r_comment, COMMENT_LEN);

            return 0;
        }

        current = current->next;
    }

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
        /* update meta data */
        metaUpdate(inex, current, NULL);

        /* delete */
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
            /* update meta data */
            metaUpdate(inex, current->next, NULL);

            /* delete */
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
    int count       = 0;
    int no_of_rec   = 0;
    long income     = 0;
    long expense    = 0;

    if (inex == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    /*
     * If no additional argument for 'view' command 
     * show maximum of 15 records only 
     */
    if (argument == NULL) {
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

    /* 
     * loop through all the records from the head 
     * terminate based on the count value 
     * if count value is -ve, show all 
     */
    while(current != NULL && (count != 0)) {
        no_of_rec++;
        if (current->rec.r_info & 1) {
            income += current->rec.r_amount;
        } else {
            expense += current->rec.r_amount;
        }

        printRecord(&current->rec);
        current = current->next;
        if (count > 0)
            count--;
    }

    puts("");
    printCalculation(no_of_rec, income, expense);

    printRecordFooter();

    return 0;
}


/*
 * Generic Filter function to call the specific filter function 
 * based on the fieldName mentioned by the user
 */
int filterRecord(InexDataPtr inex, char **token)
{
    int index = 0;

    if (inex == NULL || token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    /* if the first token itself is not 'filter', something is wrong */
    if (token[0] == NULL || strcmp(token[0], "filter") != 0) {
        logError(ERROR_SOMETHING);
        return -1;
    }

    /* loop through filter lookup */
    while (filter_lookup[index].fieldName != NULL) {
        if (token[1] == NULL) 
            return 1;

        /* 
         * if the 2nd token matches with the filter lookup fieldName 
         * call the corresponding filter function based on fiedlName
         */
        if (strcmp(filter_lookup[index].fieldName, token[1]) == 0) {
            return filter_lookup[index].filter(inex, token);
        }

        index++;
    }

    return 1;
}


void showFileName(InexDataPtr inex) 
{
    if (inex == NULL)
        return;

    printf("%s",inex->meta.md_file_name);
}


/*
 * Function to read inex data from file 
 */
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


/*
 * Function to write inex data into file 
 */
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


/*
 * Function to check if the given file name is valid or not
 * Note: FileName should be without extention
 *
 * return 1 - valid fileName 
 */
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

        /* only 25 characters are allowed */
        if (index >= (FILE_NAME_LEN - 7))
            return 0;

        /*
         * (0 - 9) is allowed
         * (a - z) is allowed
         * (A - Z) is allowed
         * underscore (_) and hyphen (-) is allowed
         */
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


/*
 * To check if a file with given fileName exist or not
 *
 * return 1 - definitely exist (100%)
 */
static int fileExist(const char *fileName) 
{
    FILE *fp;

    if (fileName == NULL) {
        logError(ERROR_ARGUMENT);
        return 0;
    }

    fp = fopen(fileName, "rb");

    if (fp == NULL)
        return 0;

    fclose(fp);

    return 1;
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

    /* 
     * Subtract the existing amount (for both delete and edit)
     */
    if (node->rec.r_info & 1) {
        inex->meta.md_total_income -= node->rec.r_amount;
    } else {
        inex->meta.md_total_expense -= node->rec.r_amount;
    }

    /* 
     * if rec is NULL, then it means only delete. 
     * so, decrease no of record count 
     */
    if (rec == NULL)
        inex->meta.md_record_count--;

    /* 
     * if rec is NOT NULL, then it means edit 
     */
    if (rec != NULL) {
        if (rec->r_amount >= 0) {
            /* 
             * if Amount field going to be edited, 
             * subtract existing value from income or expense (already did)
             * add the new value into income or expense (below)
             */
            if (node->rec.r_info & 1) {
                inex->meta.md_total_income += rec->r_amount;
            } else {
                inex->meta.md_total_expense += rec->r_amount;
            }
        } else {
            /* 
             * if No changes in Amount field during edit, 
             * restore the subtracted value in the first step (because no changes)
             */
            if (node->rec.r_info & 1) {
                inex->meta.md_total_income += node->rec.r_amount;
            } else {
                inex->meta.md_total_expense += node->rec.r_amount;
            }
        }
    }

    return 0;
}


/*
 * Function to print the metaData based on the function argument data 
 */
static void printCalculation(int no_of_rec, long income, long expense) 
{
    long balance = income - expense;

    printf("\tNo of records : %d\n", no_of_rec);
    printf("\tTotal Income  : %ld.%02ld\n", income / 100, income % 100);
    printf("\tTotal Expense : %ld.%02ld\n", expense / 100, expense % 100);
    printf("\tBalance       : %ld.%02ld\n", balance / 100,
        (balance < 0) ? (balance % 100) * (-1) : (balance % 100));
} 


/*
 * Function to filter records based on Date field
 */
static int filterByDate(InexDataPtr inex, char **token)
{
    ListNodePtr current;
    Date upper_date, lower_date;
    Date *upper, *lower;

    /* for calculation based on the filtered output */
    int no_of_rec   = 0;
    long income     = 0;
    long expense    = 0;

    if (inex == NULL || token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (token[2] == NULL || token[3] == NULL) 
        return -1;
    
    /* 
     * The upper limit should be a valid date or dot (.) 
     * if valid date -> parse the value into date format, store address in upper
     * else if dot (.) -> make upper as NULL
     * otherwise return
     */
    if (parseStringToDate(token[2], &upper_date) != 0) {
        if (strcmp(token[2],".") != 0)
            return 1;
        
        upper = NULL;
    } else {
        upper = &upper_date;
    }
        
    /* 
     * The lower limit should be a valid date or dot (.) 
     * if valid date -> parse the value into date format, store address in lower
     * else if dot (.) -> make lower as NULL
     * otherwise return
     */
    if (parseStringToDate(token[3], &lower_date) != 0) {
        if (strcmp(token[3],".") != 0)
            return 1;
        
        lower = NULL;
    } else {
        lower = &lower_date;
    }

    current = inex->headNode;
    printRecordHeader();

    /* loop through every records */
    while (current != NULL) {
        /* if the current record falls in filter range, proceed further */
        if (isRecordUnderDateRange(&current->rec, upper, lower)) {
            no_of_rec++;

            if (current->rec.r_info & 1) {
                income += current->rec.r_amount;
            } else {
                expense += current->rec.r_amount;
            }

            printRecord(&current->rec);
        }

        current = current->next;
    }

    puts("");
    printCalculation(no_of_rec, income, expense);
    printRecordFooter();

    return 0;
}


/*
 * Function to filter records based on Amount field
 */
static int filterByAmount(InexDataPtr inex, char **token)
{
    ListNodePtr current;
    long upper_amount, lower_amount;
    long *upper, *lower;

    /* for calculation based on the filtered output */
    int no_of_rec   = 0;
    long income     = 0;
    long expense    = 0;

    if (inex == NULL || token == NULL) {
        logError(ERROR_ARGUMENT);
        return -1;
    }

    if (token[2] == NULL || token[3] == NULL)
        return -1;

    /* 
     * The upper limit should be a valid amount or dot (.) 
     * if valid amount -> parse the value as Amount format, store address in upper
     * else if dot (.) -> make upper as NULL
     * otherwise return
     */
    if (parseStringToAmount(token[2], &upper_amount) != 0) {
        if (strcmp(token[2],".") != 0)
            return 1;
        
        upper = NULL;
    } else {
        upper = &upper_amount;
    }
        
    /* 
     * The lower limit should be a valid amount or dot (.) 
     * if valid amount -> parse the value as Amount format, store address in lower
     * else if dot (.) -> make lower as NULL
     * otherwise return
     */
    if (parseStringToAmount(token[3], &lower_amount) != 0) {
        if (strcmp(token[3],".") != 0)
            return 1;
        
        lower = NULL;
    } else {
        lower = &lower_amount;
    }

    current = inex->headNode;
    printRecordHeader();

    /* loop through every records */
    while (current != NULL) {
        /* if the current record falls in filter range, proceed further */
        if (isRecordUnderAmountRange(&current->rec, upper, lower)) {
            no_of_rec++;

            if (current->rec.r_info & 1) {
                income += current->rec.r_amount;
            } else {
                expense += current->rec.r_amount;
            }

            printRecord(&current->rec);
        }

        current = current->next;
    }

    puts("");
    printCalculation(no_of_rec, income, expense);
    printRecordFooter();

    return 0;
}


/*
 * Function to check if the Date of the record falls between the range
 * Note: Either upper or lower range limit can be ignored
 * 
 * return 1 - indicates record falls under the range
 */
static int isRecordUnderDateRange(struct record *rec, Date *d1, Date *d2)
{
    if (rec == NULL)
        return 0;

    /* Both upper and lower limit cannot be ignored with dot(.) */
    if (d1 == NULL && d2 == NULL)
        return 0;

    if (d1 != NULL) {
        /* if record data is lower than the lower limit, fail */
        if (compareDate(rec->r_date, *d1) < 0)
            return 0;
    }

    if (d2 != NULL) {
        /* if record data is upper than the upper limit, fail */
        if (compareDate(rec->r_date, *d2) > 0)
            return 0;
    }

    return 1;
}


/*
 * Function to check if the Amount of the record falls between the range
 * Note: Either upper or lower range limit can be ignored
 * 
 * return 1 - indicates record falls under the range
 */
static int isRecordUnderAmountRange(struct record *rec, long *a1, long *a2)
{
    if (rec == NULL)
        return 0;

    /* Both upper and lower limit cannot be ignored with dot (.) */
    if (a1 == NULL && a2 == NULL)
        return 0;

    if (a1 != NULL) {
        /* if record data is lower than the lower limit, fail */
        if (rec->r_amount < *a1)
            return 0;
    }

    if (a2 != NULL) {
        /* if record data is upper than the upper limit, fail */
        if (rec->r_amount > *a2)
            return 0;
    }

    return 1;
}


/*
 * Temporary way of printing records
 */
static int printRecord(struct record *rec) 
{
    static char type[4];
    static const char *rec_footer = 
        "\n|-----|"
        "-------|"
        "-----------------|"
        "------------|"
        "---------------------------------|";
    static const char *empty_block = 
        "\n|     | ";

    if (rec == NULL) {
        //logError(ERROR_ARGUMENT);
        return -1;
    }

    strcpy(type,"");

    if (rec->r_info & 1)
        strcpy(type,"+IN");

    printf("| %3s | %5d   %12ld.%02ld   %04d-%02d-%02d   %-31s  "
        , type, rec->r_id, (rec->r_amount / 100), (rec->r_amount % 100)
        , rec->r_date.year, rec->r_date.month, rec->r_date.day
        , rec->r_entity);

    printf("%s",empty_block);
    printf("%s",empty_block);
    printf("COMMENT : ");
    printComment(rec->r_comment);
    printf("%s",empty_block);
        
    puts(rec_footer);

    return 0;
} 


/*
 * Temporary record header
 */
static void printRecordHeader() 
{
    static const char *record_header =
        "\n\t<-----LIST OF RECORDS----->\n"

        "\n|-----|"                             // 7
        "-------|"                              // 13 - 5 = 8
        "-----------------|"                    // 21 - 3 = 18
        "------------|"                         // 13
        "---------------------------------|"    // 34

        "\n| ??? |"
        "    ID |"
        "          AMOUNT |"
        "       DATE |"
        " ENTITY                          |"

        "\n|-----|"
        "-------|"
        "-----------------|"
        "------------|"
        "---------------------------------|";

    puts(record_header);
}


/*
 * Temporary record footer
 */
static void printRecordFooter() 
{
    static const char *record_footer =
        "\n\t<-------END OF LIST------->\n";
        
    puts(record_footer);
} 


/*
 * Temporary method to print comment section
 */
static void printComment(const char *comment)
{
    int index = 0;
    static const char *initial_space =
        "\n|     |           ";

    if (comment == NULL)
        return;

    while (comment[index] != '\0') {
        if (index % 60 == 0 && index != 0)
            printf("%s", initial_space);

        printf("%c", comment[index]);

        index++;
    }
    
} 