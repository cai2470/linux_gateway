#ifndef __OTA_H
#define __OTA_H
#include "ota_http.h"
#include <openssl/sha.h>
#include "time.h"
void ota_version_check(void);
void ota_auto_update(void);
#endif
