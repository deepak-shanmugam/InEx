/*
 * customError.c
 *
 *  Created on: 28-Jun-2024
 *      Author: deepaks
 */
 
#include <stdio.h>

#include "headers/customError.h"


void logError(ErrorCode err) 
{
    fprintf(stderr, "\n\t");

    switch (err) {
        case ERROR_FUNC_ARG:
            fprintf(stderr, "ERROR: function argument error");
            break;
        case ERROR_STD_INPUT:
            fprintf(stderr, "ERROR: standard input error");
            break;
        case ERROR_MEMORY_ALLOC:
            fprintf(stderr, "ERROR: memory allocation error");
            break;
        case ERROR_FILE_OPEN:
            fprintf(stderr, "ERROR: error opening file");
            break;
        case ERROR_FILE_READ:
            fprintf(stderr, "ERROR: file read error");
            break;
        case ERROR_FILE_WRITE:
            fprintf(stderr, "ERROR: file write error");
            break;
        case ERROR_FILE_REMOVE:
            fprintf(stderr, "ERROR: error removing file");
            break;
        case ERROR_WENT_WRONG:
            fprintf(stderr, "ERROR: something went wrong");
            break;
        default:
            fprintf(stderr, "ERROR: undefined error");
            break;
    }

    fprintf(stderr, "\n");
} 