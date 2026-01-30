#include "app_msg.h"

void app_msg_json_2_msg(char *json, app_msg_t *msg)
{
    cJSON *root = cJSON_Parse(json);
    if (root == NULL)
    {
        return;
    }
    msg->connType = strdup(cJSON_GetObjectItem(root, "connType")->valuestring);
    msg->action = strdup(cJSON_GetObjectItem(root, "action")->valuestring);
    msg->motorId = cJSON_GetObjectItem(root, "motorId")->valueint;

    cJSON *motorSpeedItem = cJSON_GetObjectItem(root, "motorSpeed");
    if (motorSpeedItem != NULL)
    {
        msg->motorSpeed = motorSpeedItem->valueint;
    }
    cJSON *statusItem = cJSON_GetObjectItem(root, "status");
    if (statusItem != NULL)
    {
        msg->status = strdup(statusItem->valuestring);
    }
    cJSON_Delete(root);
}

void app_msg_msg_2_json(app_msg_t *msg, char *json)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "connType", msg->connType);
    cJSON_AddStringToObject(root, "action", msg->action);
    cJSON_AddNumberToObject(root, "motorId", msg->motorId);
    cJSON_AddNumberToObject(root, "motorSpeed", msg->motorSpeed);
    if (msg->status != NULL)
    {
        cJSON_AddStringToObject(root, "status", msg->status);
    }

    char *str = cJSON_PrintUnformatted(root);
    strcpy(json, str);
    json[strlen(str)] = '\0';

    cJSON_Delete(root);
}
