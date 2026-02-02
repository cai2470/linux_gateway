#include "ota.h"

#define OTA_VERSION_URL "http://192.168.54.188:8000/update/version_info.json"
#define CURR_INFO "{\"major\":1,\"minor\":0,\"patch\":0}"

typedef struct
{
    int major;
    int minor;
    int patch;
    char *sha1;
} ota_info_t;

void parse_version_info(char *json_str, ota_info_t *info)
{
    cJSON *root = cJSON_Parse(json_str);
    info->major = cJSON_GetObjectItem(root, "major")->valueint;
    info->minor = cJSON_GetObjectItem(root, "minor")->valueint;
    info->patch = cJSON_GetObjectItem(root, "patch")->valueint;

    cJSON *sha1Item = cJSON_GetObjectItem(root, "sha1");
    if (sha1Item != NULL)
    {
        char *sha1 = sha1Item->valuestring;
        info->sha1 = strdup(sha1);
    }
    cJSON_Delete(root);
}

void ota_version_check(void)
{
    // 1. 获取远程版本信息
    char *new_ver = ota_http_get_text(OTA_VERSION_URL);
    // log_info("new_ver: %s", new_ver);
    ota_info_t new_info;
    ota_info_t curr_info;

    parse_version_info(new_ver, &new_info);
    parse_version_info(CURR_INFO, &curr_info);

    // 打印新旧版本信息
    log_info("new_info: %d.%d.%d, %s", new_info.major, new_info.minor, new_info.patch, new_info.sha1);
    log_info("curr_info: %d.%d.%d", curr_info.major, curr_info.minor, curr_info.patch);

    // 2. 判断版本号是否需要升级
    if (new_info.major < curr_info.major ||
        (new_info.major == curr_info.major && new_info.minor < curr_info.minor) ||
        (new_info.major == curr_info.major && new_info.minor == curr_info.minor && new_info.patch <= curr_info.patch))
    {
        log_info("不需要升级");
        return;
    }

    log_info("可以升级");
    // 3. 下载
}
