#include "ota_http.h"

// 参数1: 存储数据的指针
// 参数2: size_t size: 每次返回的数据块大小
// 参数3: size_t nmemb: 返回的数据块个数
// 参数4: void *userdata: 自定义参数 可以传递一个缓冲区过来, 然后把收到的数据copy到这个缓冲区
// 返回值: size_t: 返回本次调用所接收到的数据块大小
size_t get_text_cb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t size_all = size * nmemb;
    memcpy(userdata, ptr, size_all);
    ((char *)userdata)[size_all] = '\0';
    return size_all;
}

char *ota_http_get_text(char *url)
{
    CURL *curl;
    // 做全局初始化: 整个进程, 只需要执行一次
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    // 初始化curl,
    curl = curl_easy_init();

    if (curl == NULL)
    {
        log_error("curl_easy_init() failed");
        return NULL;
    }
    // 设置请求url地址
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // 设置回调函数,用来接收服务器返回的数据
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_text_cb);

    char *text = (char *)malloc(1024);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, text);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK)
    {
        log_error("curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        return NULL;
    }
    curl_easy_cleanup(curl);
    return text;
}

gate_state_t ota_http_download(char *url, char *file_name)
{
    CURL *curl;
    curl = curl_easy_init();
    if (curl == NULL)
    {
        log_error("curl_easy_init() failed");
        return GATE_ERROR;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
    FILE *fp = fopen(file_name, "wb");
    if (fp == NULL)
    {
        log_error("fopen() failed");
        curl_easy_cleanup(curl);
        return GATE_ERROR;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        log_error("curl_easy_perform() failed: %s", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        return GATE_ERROR;
    }
    curl_easy_cleanup(curl);
    fclose(fp);  // 关闭文件
    return GATE_OK;
}
