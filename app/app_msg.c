#include "app_msg.h"
#include <string.h>
#include <stdlib.h>

// JSON 转 结构体
void app_msg_json_2_msg(char *json, app_msg_t *msg)
{
    if (json == NULL || msg == NULL) {
        return;
    }

    cJSON *root = cJSON_Parse(json);
    if (root == NULL)
    {
        // 解析失败，可以添加日志打印
        return;
    }

    // 1. 解析 id (数字)
    cJSON *item = cJSON_GetObjectItem(root, "id");
    if (cJSON_IsNumber(item)) {
        msg->id = item->valueint;
    }

    // 2. 解析 connectType (字符串)
    item = cJSON_GetObjectItem(root, "connectType");
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
        // 注意：这里使用了 strdup，后续需要记得 free msg->connectType
        msg->connectType = strdup(item->valuestring);
    } else {
        msg->connectType = NULL;
    }

    // 3. 解析 type (字符串)
    item = cJSON_GetObjectItem(root, "type");
    if (cJSON_IsString(item) && (item->valuestring != NULL)) {
        msg->type = strdup(item->valuestring);
    } else {
        msg->type = NULL;
    }

    // 4. 解析 is_start (数字)
    item = cJSON_GetObjectItem(root, "is_start");
    if (cJSON_IsNumber(item)) {
        msg->is_start = item->valueint;
    }

    // 5. 解析 targetAngle (数字)
    item = cJSON_GetObjectItem(root, "targetAngle");
    if (cJSON_IsNumber(item)) {
        msg->targetAngle = item->valueint;
    }

    // 6. 解析 targetSpeed (数字)
    item = cJSON_GetObjectItem(root, "targetSpeed");
    if (cJSON_IsNumber(item)) {
        msg->targetSpeed = item->valueint;
    }

    cJSON_Delete(root);
}

// 结构体 转 JSON
void app_msg_msg_2_json(app_msg_t *msg, char *json)
{
    if (msg == NULL || json == NULL) {
        return;
    }

    cJSON *root = cJSON_CreateObject();

    // 添加所有字段
    cJSON_AddNumberToObject(root, "id", msg->id);

    if (msg->connectType != NULL) {
        cJSON_AddStringToObject(root, "connectType", msg->connectType);
    }

    if (msg->type != NULL) {
        cJSON_AddStringToObject(root, "type", msg->type);
    }

    cJSON_AddNumberToObject(root, "is_start", msg->is_start);
    cJSON_AddNumberToObject(root, "targetAngle", msg->targetAngle);
    cJSON_AddNumberToObject(root, "targetSpeed", msg->targetSpeed);

    // 生成 JSON 字符串
    char *str = cJSON_PrintUnformatted(root);
    if (str != NULL) {
        strcpy(json, str); // 确保传入的 json 缓冲区足够大
        free(str);         // 关键：释放 cJSON 分配的内存，防止内存泄漏
    }

    cJSON_Delete(root);
}