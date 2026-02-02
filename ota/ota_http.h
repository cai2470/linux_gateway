#ifndef __OTA_HTTP_H
#define __OTA_HTTP_H
#include "app_common.h"
#include "curl/curl.h"
char *ota_http_get_text(char *url);
gate_state_t ota_http_download(char *url, char *file_name);
#endif
