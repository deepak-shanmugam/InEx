#ifndef CUSTOM_ERROR_H
#define CUSTOM_ERROR_H

typedef enum {
    ERROR_FUNC_ARG,
    ERROR_STD_INPUT,
    ERROR_MEMORY_ALLOC,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_FILE_WRITE,
    ERROR_FILE_REMOVE,
    ERROR_WENT_WRONG
} ErrorCode;

void logError(ErrorCode err);

#endif 