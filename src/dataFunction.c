/*
 * customInput.c (Renamed as dataFunction.c)
 *
 *  created date: 21-Jul-2024
 *        Auther: deepaks
 */ 

#include <stdio.h>
#include <string.h>

#include "headers/dataFunction.h"

#define BUFFER_LEN  32  // Should be greater than the below macro values
#define INT_LEN     12
#define LONG_LEN    22
#define DATE_LEN    11
#define AMOUNT_LEN  22

static char buffer[BUFFER_LEN];


/*
 * Function to store String input value from console into input variable
 *
 * Return >= 0, indicates success, the no.of characters entered in console
 * Return < 0, indicates Error or invalid function argument values
 */
int getStringFromConsole(char *input, int length) 
{
    int returnCode;
    char ch;

    if (input == NULL || length <= 0)
        return -2;

    if (fgets(input, length, stdin) == NULL)
        return -1;

    returnCode = strcspn(input,"\n");
    input[returnCode] = '\0';

    if (returnCode >= (length - 1)) {
        while((ch = getchar()) != '\n' && ch != EOF) {
            returnCode++;
        }
    }

    return returnCode;
} 


/*
 * Function to store Int input value from console into input variable
 *
 * Return > 0, indicates Successful
 * Return = 0, indicates invalid input value
 * Return < 0, indicates Error or invalid function argument values
 */
int getIntFromConsole(int *input)
{
    int returnCode;

    if (input == NULL)
        return -2;

    returnCode = getStringFromConsole(buffer, INT_LEN);
    if (returnCode <= 0) 
        return returnCode;

    return sscanf(buffer,"%d",input);
} 


/*
 * Function to store Long input value from console into input variable
 *
 * Return > 0, indicates Successful
 * Return = 0, indicates invalid input value
 * Return < 0, indicates Error or invalid function argument values
 */
int getLongFromConsole(long *input)
{
    int returnCode;

    if (input == NULL)
        return -2;

    returnCode = getStringFromConsole(buffer, LONG_LEN);
    if (returnCode <= 0) 
        return returnCode;

    return sscanf(buffer,"%ld",input);
} 


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
        if (isValidDate(date->year, date->month, date->day)) {
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
 * Function to check if the string is in valid Amount format
 *
 * Return 1 - indicates valid format
 */
int isValidStringAmount(const char *str) 
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
 * Return 1 - indicates valid date
 */
int isValidDate(int year, int month, int day) 
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
int copyRecord(struct record *dest, struct record *src) 
{
    if (dest == NULL || src == NULL) 
        return -1;

    dest->r_id       = src->r_id;
    dest->r_info     = src->r_info;
    dest->r_amount   = src->r_amount;
    dest->r_date     = src->r_date;
    strncpy(dest->r_entity, src->r_entity, ENTITY_LEN);
    strncpy(dest->r_comment, src->r_comment, COMMENT_LEN);

    return 0;
} 