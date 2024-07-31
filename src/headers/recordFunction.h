#ifndef RECORD_FUNCTION_H
#define RECORD_FUNCTION_H

#include "dataDefinition.h"


/* get Input From Console Functions */
int getDateFromConsole(Date *date);

int getAmountFromConsole(long *amount);

int getRecordFromConsole(Record *rec, int mandatoryCheck);


/* data parsing functions */
int parseStringToDate(const char *str, Date *date);

int parseStringToAmount(const char *str, long *amount);


/* data validity functions */
int isValidAmount(const long *amount);

int isValidDate(const Date *date);

int isValidRecord(const Record *rec);

int isValidRecordId(const Record *rec);

int isValidRecordInfo(const Record *rec);

int isValidRecordEntity(const Record *rec);

int isValidRecordComment(const Record *rec);

int isRecordBetweenDateRange(const Record *rec, Date *d1, Date *d2);

int isRecordBetweenAmountRange(const Record *rec, long *a1, long *a2);


/* other functions */
int compareDate(Date d1, Date d2);

int copyRecord(Record *dest, Record *src);


/* print record in console function */
int printRecordInConsole(const Record *rec);

void printRecordHeaderInConsole();

void printRecordFooterInConsole();

void printCalculationInConsole(int no_of_rec, long income, long expense);


#endif 