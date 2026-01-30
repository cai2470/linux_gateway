#ifndef __APP_COMMON_H
#define __APP_COMMON_H
#define _GNU_SOURCE  //: 放在最前面
#include "log/log.h"
#include "cjson/cJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "inttypes.h"


typedef enum
{
    GATE_OK,
    GATE_ERROR,
    GATE_TIMEOUT,
    GATE_OTHER
} gate_state_t;

#endif
