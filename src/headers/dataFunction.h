#ifndef DATA_FUNCTION_H
#define DATA_FUNCTION_H

#include "dataDefinition.h"


/* data input functions */
int getStringFromConsole(char *input, int length);

int getIntFromConsole(int *input);

int getLongFromConsole(long *input);

int getDateFromConsole(Date *date);

int getAmountFromConsole(long *amount);


/* data parsing functions */
int parseStringToDate(const char *str, Date *date);

int parseStringToAmount(const char *str, long *amount);


/* data validity functions */
int isValidStringAmount(const char *str);

int isValidDate(int year, int month, int day);

#endif 