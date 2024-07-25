/*
 * customInput.c
 *
 *  created date: 21-Jul-2024
 *        Auther: deepaks
 */ 

#include <stdio.h>
#include <string.h>

#include "headers/customInput.h"

#define BUFFER_LEN  32  // Should be greater than the below macro values
#define INT_LEN     12
#define LONG_LEN    22
#define DATE_LEN    11
#define AMOUNT_LEN  22


static int isValidDate(int day, int month, int year);
static int isValidAmountFormat(char *str);

static char buffer[BUFFER_LEN];


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


int getDateFromConsole(Date *input)
{
    int returnCode;

    if (input == NULL)
        return -1;

    returnCode = getStringFromConsole(buffer, DATE_LEN);
    if (returnCode <= 0)
        return returnCode;

    if (sscanf(buffer, "%d-%d-%d", &input->year, &input->month, &input->day) == 3) {
        if (isValidDate(input->day, input->month, input->year)) {
            return 1;
        }
    }

    return 0;
} 


int getAmountFromConsole(long *input)
{
    long main, deci;
    int returnCode;

    if (input == NULL)
        return -1;

    returnCode = getStringFromConsole(buffer, AMOUNT_LEN);
    if (returnCode <= 0)
        return returnCode;

    if (isValidAmountFormat(buffer) == 0)
        return 0;

    main = 0;
    deci = 0;
    if (sscanf(buffer,"%ld.%ld",&main,&deci) <= 0) 
        return 0;

    if ((main < 0 || main > 999999999999) 
            || (deci < 0 || deci > 99))
        return 0;

    *input = (main * 100) + deci;

    return 1;
} 


static int isValidDate(int day, int month, int year) 
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


static int isValidAmountFormat(char *str) 
{
    int i, point = 0, pos = 0;

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