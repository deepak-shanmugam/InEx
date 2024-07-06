#ifndef INEX_DATA_H
#define INEX_DATA_H

typedef struct inexData* InexDataPtr;


InexDataPtr createInexData(const char *fileName);

InexDataPtr openInexDataFromFile(const char *fileName);

int saveInexData(InexDataPtr inex);

void showFileName(InexDataPtr inex);

int removeInexFile(const char *fileName);

void destroyInexData(InexDataPtr inex);

#endif 