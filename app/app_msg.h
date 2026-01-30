#ifndef __APP_MSG_H
#define __APP_MSG_H
#include "app_common.h"

typedef struct
{
    char *connType;
    int motorId;
    char *action;
    int16_t motorSpeed;
    char *status;
} app_msg_t;

void app_msg_json_2_msg(char *json, app_msg_t *msg);
void app_msg_msg_2_json(app_msg_t *msg, char *json);
#endif
