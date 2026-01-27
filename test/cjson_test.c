#include "log/log.h"
#include "cjson/cJSON.h"

void json_parse()
{
    char *json = "{\"name\":\"zhangsan\",\"age\":18}";

    cJSON *root = cJSON_Parse(json);

    if (root == NULL)
    {
        log_error("json parse error");
        return;
    }

    cJSON *name = cJSON_GetObjectItem(root, "name");

    log_info("name: %s", name->valuestring);

    cJSON *age = cJSON_GetObjectItem(root, "age");
    log_info("age: %d", age->valueint);

    cJSON_Delete(root);
}

void json_tostring()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "zhangsan");
    cJSON_AddNumberToObject(root, "age", 18);
    //log_info("json: %s", cJSON_Print(root));

    log_info("json: %s", cJSON_PrintUnformatted(root));

    cJSON_Delete(root);
}

int main(int argc, char const *argv[])
{
    // 设置日志级别
    log_set_level(LOG_INFO);

    // json_parse();
    json_tostring();
    return 0;
}
/*
LOG_TRACE,
LOG_DEBUG,
LOG_INFO,
LOG_WARN,
LOG_ERROR,
LOG_FATAL


*/
