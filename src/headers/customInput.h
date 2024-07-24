#ifndef CUSTOM_INPUT_H
#define CUSTOM_INPUT_H 

#include "dataDefinition.h"

int getStringFromConsole(char *input, int length);

int getIntFromConsole(int *input);

int getLongFromConsole(long *input);

int getDateFromConsole(Date *input);

int getAmountFromConsole(long *input);

#endif 