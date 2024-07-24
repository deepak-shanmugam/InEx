/*
 * appInfo.c
 *
 *  Created on: 1-Jul-2024
 *      Author: deepaks
 */
 
#include <stdio.h>

#include "headers/appInfo.h"

static void show_license(void);


void about(void) 
{ 
    static const char *about_text = 
        "\n\tInEx - CLI based Income and Expense tracking application"
        "\n\tcopyright (c) 2024, Deepak Shanmugam"
        "\n\tLicense: GNU GENERAL PUBLIC LICENSE"
        "\n\tversion: 0.0.1 (ALPHA)"
        "\n\tcontact: deepdeepdeepak@outlook.com";

    puts(about_text);
    show_license();
} 


void help(char **token) 
{
    if (token == NULL)
        return;

    static const char *help_text =
        "\n<----START OF HELP MENU---->\n"
        "\nquit\n"
            "\t- to quit or exit the application\n"
            "\t- FORMAT: quit\n"
        "\nhelp\n"
            "\t- to know about available commands\n"
            "\t- FORMAT: help\n"
        "\nabout\n"
            "\t- about the application\n"
            "\t- FORMAT: about\n"
        "\ncreate\n"
            "\t- to create a new inex file\n"
            "\t- FORMAT: create <file_name>\n"
            "\t- <file_name> contains alphanumerics, hyphen(-) and underscore(_)\n"
            "\t- <file_name> maximum of 25 characters\n"
            "\t- <file_name> without extension\n"
        "\nopen\n"
            "\t- to open an existing inex file\n"
            "\t- FORMAT: open <file_name>\n"
            "\t- <file_name> contains alphanumerics, hyphen(-) and underscore(_)\n"
            "\t- <file_name> maximum of 25 characters\n"
            "\t- <file_name> without extension\n"
        "\nremove\n"
            "\t- to remove an existing inex file.\n"
            "\t- FORMAT: remove <file_name>\n"
            "\t- <file_name> contains alphanumerics, hyphen(-) and underscore(_)\n"
            "\t- <file_name> maximum of 25 characters\n"
            "\t- <file_name> without extension\n"
        "\nlist\n"
            "\t- to list all the inex file in current directory\n"
            "\t- FORMAT: list\n"
        "\nadd\n"
            "\t- to add an income or an expense record\n"
            "\t- FORMAT: add <in/ex>\n"
            "\tflag: in, to add income\n"
            "\tflag: ex, to add expense\n"
        "\nedit\n"
            "\t- to edit a record using its id\n"
            "\t- FORMAT: edit <id>\n"
            "\t- <id> indicates the unique id of a record\n"
        "\ndelete\n"
            "\t- to delete a record using its id\n"
            "\t- FORMAT: delete <id>\n"
            "\t- <id> indicates the unique id of a record\n"
        "\nview\n"
            "\t- to view all or given number of records\n"
            "\t- FORMAT: view <flag>/<count>\n"
            "\t- Without <flag>/<count>, top 15 records by default\n"
            "\t- <count> indicates the number of record to view\n"
            "\tflag: all, to view all records\n"
        "\nfilter\n"
            "\t- to view records based on the given range values\n"
            "\t- FORMAT: filter <flag> <min_value> <max_value> <in/ex>\n"
            "\tflag: in/ex, to apply filter on income or expense record\n"
                "\t\t- this is optional\n"
                "\t\t- it will consider all records, if not included\n"
            "\tflag: amount, to apply filter based on amount values\n"
            "\tflag: date, to apply filter based on date values\n"
                "\t\t- dot(.) is used to ignore either <min_value> or <max_value>\n"
                "\t\t- dot(.) cannot be used for both <min_value> and <max_value>\n"
        "\ninfo\n"
            "\t- to show the meta data of current inex file\n"
            "\t- FORMAT: info\n"
        "\nsave\n"
            "\t- to save the current inex file\n"
            "\t- FORMAT: save\n"
        "\nclose\n"
            "\t- to close the current inex file\n"
            "\t- FORMAT: close\n"
        "\n<----END OF HELP MENU---->\n";

    puts(help_text);
}


static void show_license(void) 
{
    static const char *license_text =
        "\n\tLICENSE: This program is free software: "
        "you can redistribute it and/or modify it "
        "under the terms of the GNU General Public License "
        "as published by the Free Software Foundation, "
        "either version 3 of the License, or (at your option) any later version.\n"

        "\n\tThis program is distributed in the hope that it will be useful, "
        "but WITHOUT ANY WARRANTY; "
        "without even the implied warranty of MERCHANTABILITY "
        "or FITNESS FOR A PARTICULAR PURPOSE.  "
        "See the GNU General Public License for more details.\n"

        "\n\tYou should have received a copy of the "
        "GNU General Public License along with this program.  "
        "If not, see <http://www.gnu.org/licenses/> for more info.\n";

    puts(license_text);
} 