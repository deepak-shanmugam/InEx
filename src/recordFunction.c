/*
 * recordFunction.c (renamed)
 *
 *  created date: 21-Jul-2024
 *        Auther: deepaks
 */ 

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "headers/recordFunction.h"
#include "headers/consoleInput.h"

#define BUFFER_LEN  32  // Should be greater than the below macro values
#define DATE_LEN    11
#define AMOUNT_LEN  22


static int isValidStringAmount(const char *str);
static int isValidDateField(int year, int month, int day);
static void printCommentInConsole(const char *comment);


static char buffer[BUFFER_LEN];

static const char *record_header =
    "\n\t<------LIST OF RECORDS------>\n";
static const char *header_column_text = 
    " ??? |          AMOUNT |       DATE | ENTITY ";
static const char *row_seperator_text = 
    "-----|-----------------|------------|---------------------------------|";
static const char *record_footer =
    "\n\t<--------END OF LIST-------->\n";


/*
 * Function to store Date input value from console into date variable
 *
 * Return > 0, indicates Successful
 * Return = 0, indicates invalid input value
 * Return < 0, indicates Error or invalid function argument values
 */
int getDateFromConsole(Date *date)
{
    int returnCode;

    if (date == NULL)
        return -1;

    returnCode = getStringFromConsole(buffer, DATE_LEN);
    if (returnCode <= 0)
        return returnCode;

    if (parseStringToDate(buffer, date) == 0)
        return 1;

    return 0;
} 


/*
 * Function to store Amount input value from console into amount variable
 *
 * Return > 0, indicates Successful
 * Return = 0, indicates invalid input value
 * Return < 0, indicates Error or invalid function argument values
 */
int getAmountFromConsole(long *amount)
{
    int returnCode;

    if (amount == NULL)
        return -1;

    returnCode = getStringFromConsole(buffer, AMOUNT_LEN);
    if (returnCode <= 0)
        return returnCode;

    if (parseStringToAmount(buffer, amount) == 0)
        return 1;

    return 0;
} 


/*
 * Function to get record from the user
 *
 * Return = 0, indicates success
 * Return > 0, indicates Invalid Input Value
 * Return < 0, indicates Error 
 */
int getRecordFromConsole(Record *rec, int mandatoryCheck) 
{
    int returnCode;

    if (rec == NULL)
        return -2;

    printf("   %cAmount            : ", (mandatoryCheck != 0) * 42);
    returnCode = getAmountFromConsole(&rec->r_amount);
    if (returnCode < 0)
        return -1;
    if (returnCode == 0) {
        rec->r_amount = -1;     // indication to skip during edit 
        if (mandatoryCheck)
            return 1;
    }

    printf("   %cDate [yyyy-mm-dd] : ", (mandatoryCheck != 0) * 42);
    returnCode = getDateFromConsole(&rec->r_date);
    if (returnCode < 0) 
        return -1;
    if (returnCode == 0) {
        rec->r_date.day = 0;    // indication to skip during edit 
        if (mandatoryCheck)
            return 2;
    }

    printf("   %cEntity            : ", (mandatoryCheck != 0) * 32);
    returnCode = getStringFromConsole(rec->r_entity, ENTITY_LEN);
    if (returnCode < 0)
        return -1; 

    printf("   %ccomment           : ", (mandatoryCheck != 0) * 32);
    returnCode = getStringFromConsole(rec->r_comment, COMMENT_LEN);
    if (returnCode < 0) 
        return -1; 

    return 0;
} 


/*
 * Function to parse string into Date
 *
 * Return = 0 - Success
 * Return > 0 - Invalid input scenario
 * Return < 0 - Error or Invalid function argument values
 */
int parseStringToDate(const char *str, Date *date)
{
    if (str == NULL || date == NULL)
        return -1;

    if (sscanf(str, "%d-%d-%d", &date->year, &date->month, &date->day) == 3) {
        if (isValidDate(date)) {
            return 0;
        }
    }

    return 1;
}


/*
 * Function to parse string into amount
 *
 * Return = 0 - Success
 * Return > 0 - Invalid input scenario
 * Return < 0 - Error or Invalid function argument values
 */
int parseStringToAmount(const char *str, long *amount) 
{
    long temp_main = 0;
    long temp_deci = 0;

    if (str == NULL || amount == NULL) 
        return -1;

    if (isValidStringAmount(str) == 0)
        return 1;
    
    if (sscanf(str, "%ld.%ld", &temp_main, &temp_deci) <= 0) 
        return 2;

    if (temp_main < 0 || temp_main > 999999999999 ||
            temp_deci < 0 || temp_deci > 99) 
        return 3;

    *amount = (temp_main * 100) + temp_deci;

    return 0;
} 


/*
 * return value of non-zero, indicates valid one
 */
int isValidDate(const Date *date)  
{
    if (date == NULL)
        return 0;

    return isValidDateField(date->year, date->month, date->day);
}


/*
 * return value of non-zero, indicates valid one
 */
int isValidAmount(const long *amount) 
{
    if (amount == NULL)
        return 0;

    if (*amount >= 0 && *amount <= MAX_AMOUNT)
        return 1;

    return 0;
}


/*
 * return value of non-zero, indicates valid one
 */
int isValidRecord(const Record *rec)
{
    if (rec == NULL)
        return 0;

    if (isValidRecordId(rec) == 0)
        return 0;

    if (isValidRecordInfo(rec) == 0)
        return 0;

    if (isValidDate(&rec->r_date) == 0)
        return 0;

    if (isValidAmount(&rec->r_amount) == 0)
        return 0;

    if (isValidRecordEntity(rec) == 0)
        return 0;

    if (isValidRecordComment(rec) == 0)
        return 0;
    
    return 1;
} 


/*
 * return value of non-zero, indicates valid one
 */
int isValidRecordId(const Record *rec) {
    if (rec == NULL)
        return 0;

    if (rec->r_id >= 0 && rec->r_id < INT_MAX)
        return 1;

    return 0;
}


/*
 * return value of non-zero, indicates valid one
 */
int isValidRecordInfo(const Record *rec) {
    if (rec == NULL)
        return 0;

    if (rec->r_info >= 0 && rec->r_info < INT_MAX)
        return 1;

    return 0;
}


/*
 * return value of non-zero, indicates valid one
 */
int isValidRecordEntity(const Record *rec) {
    if (rec == NULL)
        return 0;

    if (strnlen(rec->r_entity, ENTITY_LEN) < ENTITY_LEN)
        return 1;

    return 0;
}


/*
 * return value of non-zero, indicates valid one
 */
int isValidRecordComment(const Record *rec) {
    if (rec == NULL)
        return 0;

    if (strnlen(rec->r_comment, COMMENT_LEN) < COMMENT_LEN)
        return 1;

    return 0;
}


/*
 * return value of non-zero, indicates Yes or Success 
 */
int isRecordBetweenDateRange(const Record *rec, Date *d1, Date *d2)
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
 * return value of non-zero, indicates Yes or Success 
 */
int isRecordBetweenAmountRange(const Record *rec, long *a1, long *a2)
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
 * Function to compare two dates
 *
 * if d1 > d2, return 1  (d1 is latest date)
 * if d1 < d2, return -1 (d2 is latest date)
 * if d1 = d2, return 0 
 */
int compareDate(Date d1, Date d2) 
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


/*
 * Function to copy values from source record to destination record
 */
int copyRecord(Record *dest, Record *src) 
{
    if (dest == NULL || src == NULL) 
        return -1;

    /*
     * The members of the 'Record' doesn't have pointers
     * So, we can directly assign or use memcpy()
     *
     * Note: If pointers also as members, modify this implementation 
     */
    *dest = *src;

    return 0;
} 


/*
 * To print record in console with a line seperator at the end 
 */
int printRecordInConsole(const Record *rec) 
{
    static char type[4];

    if (rec == NULL) 
        return -1;

    strcpy(type," x ");

    if (rec->r_info & 1)
        strcpy(type,"+IN");

    printf(" %3s | %12ld.%02ld | %04d-%02d-%02d | %s\n\n"
        , type, (rec->r_amount / 100), (rec->r_amount % 100) 
        , rec->r_date.year, rec->r_date.month, rec->r_date.day
        , rec->r_entity);

    printf("     ID      : %d\n", rec->r_id);
    printf("     COMMENT : ");
    printCommentInConsole(rec->r_comment);
    printf("\n\n%s\n", row_seperator_text);

    return 0;
} 


/*
 * to print record header including column name in console
 */
void printRecordHeaderInConsole() 
{    
    puts(record_header);
    puts(row_seperator_text);
    puts(header_column_text);
    puts(row_seperator_text);
} 


/*
 * to print record footer in console
 */
void printRecordFooterInConsole() 
{    
    puts(record_footer);
} 


/*
 * Function to print the metaData based on the function arguments 
 */
void printCalculationInConsole(int no_of_rec, long income, long expense) 
{
    long balance = income - expense;

    printf("\tNo of records : %d\n", no_of_rec);
    printf("\tTotal Income  : %ld.%02ld\n", income / 100, income % 100);
    printf("\tTotal Expense : %ld.%02ld\n", expense / 100, expense % 100);
    printf("\tBalance       : %ld.%02ld\n", balance / 100,
        (balance < 0) ? (balance % 100) * (-1) : (balance % 100));
} 


/*
 * To check if the string is in valid Amount format
 *
 * Return != 0 (non-zero), indicates valid format
 */ 
static int isValidStringAmount(const char *str) 
{
    int i, point = 0, pos = 0;

    if (str == NULL) 
        return 0;

    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') 
            continue;

        if (str[i] == '.') {
            point++;

            if (point > 1)
                return 0;

            pos = i;
            continue;
        }

        return 0;
    }

    if (i > 15)
        return 0;

    if (point && (i - pos) != 3)
        return 0;

    return 1;
} 


/*
 * Function to check if the year, month, day are valid
 *
 * Return != 0 (non-zero), indicates valid date 
 */
static int isValidDateField(int year, int month, int day) 
{
    static const int month_day[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int isLeap = 0;

    if (year <= 0 || month <= 0 || day <= 0)
        return 0;

    if (year > 9999 || month > 12)
        return 0;

    if (month != 2) {
        if (day > month_day[month]) 
            return 0;
    } else {
        if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
            isLeap = 1;

        if (day > (month_day[month] + isLeap))
            return 0;
    }

    return 1;
} 


/*
 * to print record comment in console with some format  
 */
static void printCommentInConsole(const char *comment)
{
    int index = 0;
    static const char *blank_space =
        "\n               ";

    if (comment == NULL)
        return;

    while (comment[index] != '\0') {
        if (index % 54 == 0 && index != 0) 
            printf("%s", blank_space);

        printf("%c", comment[index]);

        index++;
    }
} 