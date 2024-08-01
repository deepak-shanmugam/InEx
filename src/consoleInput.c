/*
 * consoleInput.c
 *
 *  Created on  : 31-Jul-2024
 *  Auther      : deepaks
 */

#include <stdio.h>
#include <string.h> 

#include "headers/consoleInput.h"

#define BUFFER_LEN  32  // Should be greater than the below macro values
#define INT_LEN     12
#define LONG_LEN    22

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

    /* Before getting input, check if any feof error in terminal */
    if (feof(stdin) != 0) 
        clearerr(stdin);

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