#ifndef DATA_DEFINITION_H
#define DATA_DEFINITION_H

#define ENTITY_LEN      32
#define COMMENT_LEN     128

typedef struct date {
    int day;
    int month;
    int year;
} Date;

struct record {
    int         r_id;
    int         r_info;
    Date        r_date;
    long        r_amount;
    char        r_entity[ENTITY_LEN];
    char        r_comment[COMMENT_LEN];
};

#endif 