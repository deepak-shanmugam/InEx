#ifndef INEX_DATA_H
#define INEX_DATA_H

#include "dataDefinition.h"

/* Incomplete DataType */
typedef struct inexData* InexDataPtr;


/* InEx Data operations */
InexDataPtr createInexData(const char *fileName);

InexDataPtr openInexDataFromFile(const char *fileName);

int infoInexData(InexDataPtr inex);

int saveInexData(InexDataPtr inex);

void destroyInexData(InexDataPtr inex);


/* InEx File operations */
int removeInexFile(const char *fileName);

void listInexFile();


/* InEx Record operations */
int addRecord(InexDataPtr inex, struct record *rec);

int editRecord(InexDataPtr inex, struct record *rec);

int deleteRecord(InexDataPtr inex, int record_id);

int viewRecord(InexDataPtr inex, const char *argument);

int filterRecord(InexDataPtr inex, char **token);


/* other InEx functions */
void showFileName(InexDataPtr inex);

#endif 