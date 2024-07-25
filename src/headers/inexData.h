#ifndef INEX_DATA_H
#define INEX_DATA_H

#include "dataDefinition.h"

/* Incomplete DataType */
typedef struct inexData* InexDataPtr;


InexDataPtr createInexData(const char *fileName);

InexDataPtr openInexDataFromFile(const char *fileName);

int saveInexData(InexDataPtr inex);

void destroyInexData(InexDataPtr inex);

int removeInexFile(const char *fileName);

void listInexFile();

int addRecord(InexDataPtr inex, struct record *rec);

int editRecord(InexDataPtr inex, struct record *rec);

int deleteRecord(InexDataPtr inex, int record_id);

int viewRecord(InexDataPtr inex, const char *argument);

int filterRecord(InexDataPtr inex, char **token);

int infoInexData(InexDataPtr inex);

void showFileName(InexDataPtr inex);

#endif 