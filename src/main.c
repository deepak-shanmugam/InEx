#include <stdio.h>

#include "headers/appInterface.h"

static void app_header();


int main(int argc, char *argv[]) 
{
    AppDataPtr appData = createAppData();
    int returnCode = 1;

    app_header();

    while (returnCode) {
        returnCode = performGetCommand(appData);
        //printf("Return code: %d\n",returnCode);

        if (returnCode < 0) {
            createTemporaryBackup(appData);

            break;
        }
    }
    
    destroyAppData(appData);

    return 0;
} 


static void app_header() 
{
    puts("Welcome to InEx, copyright (c) 2024 Deepak Shanmugam");
    puts("GNU general public license");
    puts("Enter 'help' or 'about' command to know more\n");
} 