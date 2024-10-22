/*
 * client.c (renamed) 
 *
 *  Created on: 26-Jun-2024
 *      Author: deepaks
 */

#include <stdio.h>

#include "headers/appInfo.h"
#include "headers/command.h"


int main(int argc, char *argv[]) 
{
    AppDataPtr appData = createAppData();
    int returnCode = 1;

    app_header();

    while (returnCode) {
        returnCode = performGetCommand(appData);
    }
    
    destroyAppData(appData);

    return 0;
} 