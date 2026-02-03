#ifndef __APP_MSG_H
#define __APP_MSG_H
#include "app_common.h"

// app_msg.h
typedef struct {
    int id;                 // 对应 "id"
    char *connectType;      // 对应 "connectType"
    char *type;             // 对应 "type"
    int is_start;           // 对应 "is_start"
    int targetAngle;        // 对应 "targetAngle"
    int targetSpeed;        // 对应 "targetSpeed"
} app_msg_t;

void app_msg_json_2_msg(char *json, app_msg_t *msg);
void app_msg_msg_2_json(app_msg_t *msg, char *json);
#endif
