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
        case ERROR_ARGUMENT:
            fprintf(stderr, "ERROR: function argument error occurred");
            break;
        case ERROR_INPUT:
            fprintf(stderr, "ERROR: input error occurred");
            break;
        case ERROR_MEMORY_ALLOC:
            fprintf(stderr, "ERROR: memory allocation failed");
            break;
        case ERROR_FILE_OPEN:
            fprintf(stderr, "ERROR: failed to open file");
            break;
        case ERROR_FILE_READ:
            fprintf(stderr, "ERROR: error occurred during file read");
            break;
        case ERROR_FILE_WRITE:
            fprintf(stderr, "ERROR: error occurred during file write");
            break;
        case ERROR_FILE_REMOVE:
            fprintf(stderr, "ERROR: Unable to remove file");
            break;
        default:
            fprintf(stderr, "ERROR: error not defined");
            break;
    }

    fprintf(stderr, "\n");
} 