#ifndef FUNCTION_STRATEGY_H
#define FUNCTION_STRATEGY_H

#include "appInterface.h"

typedef int (*CommandFunction) (AppDataPtr appData);

typedef struct {
    const char      *command;
    CommandFunction cmdFunction;
    int             min_token;
} CommandLookup;

#endif 