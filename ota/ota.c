#include "ota.h"

#define OTA_VERSION_URL "http://192.168.54.188:8000/update/version_info.json"
#define CURR_INFO "{\"major\":1,\"minor\":0,\"patch\":0}"
#define OTA_BIN_URL "http://192.168.54.188:8000/update/gateway"
#define OTA_BIN_FILE "/usr/bin/gateway.update"

typedef struct
{
    int major;
    int minor;
    int patch;
    char *sha1;
} ota_info_t;

// 解析版本信息的json字符串
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

// 本地指纹计算器
static char *get_file_sha(char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha1;
    SHA1_Init(&sha1);

    const int bufSize = 32768;
    unsigned char *buffer = (unsigned char *)malloc(bufSize);
    if (!buffer)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    int bytesRead;
    while ((bytesRead = fread(buffer, 1, bufSize, file)) > 0)
    {
        SHA1_Update(&sha1, buffer, bytesRead);
    }

    SHA1_Final(hash, &sha1);
    fclose(file);
    free(buffer);

    char *outputBuffer = (char *)malloc(SHA_DIGEST_LENGTH * 2 + 1);
    if (!outputBuffer)
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }

    return outputBuffer;
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
    gate_state_t err = ota_http_download(OTA_BIN_URL, OTA_BIN_FILE);
    if (err != GATE_OK)
    {
        log_error("下载失败");
        return;
    }
    log_info("下载成功");
    // 4. 做hash校验
    if (strcmp(get_file_sha(OTA_BIN_FILE), new_info.sha1) != 0)
    {
        log_error("hash校验失败");
        return;
    }
    log_info("hash校验成功");
    log_info("ota升级成功");
    return;
}

void ota_auto_update(void)
{
    ota_version_check();
    while (1)
    {
        // 从1970-01-01 0:0:0 开始的s
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        if (t->tm_hour == 10 && t->tm_min == 34)
        {
            ota_version_check();
        }
        log_info("ota升级检测....");
        sleep(60);
    }
}
