#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

typedef struct appData* AppDataPtr;


AppDataPtr createAppData(void);

int performGetCommand(AppDataPtr appData);

void destroyAppData(AppDataPtr appData);

void createTemporaryBackup(AppDataPtr appData);

#endif 