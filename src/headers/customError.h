#ifndef CUSTOM_ERROR_H
#define CUSTOM_ERROR_H

typedef enum {
    ERROR_ARGUMENT,
    ERROR_INPUT,
    ERROR_MEMORY_ALLOC,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_FILE_WRITE,
    ERROR_FILE_REMOVE
} ErrorCode;

void logError(ErrorCode err);

#endif 