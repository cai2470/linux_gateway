#ifndef __APP_DEVICE_H
#define __APP_DEVICE_H
#include "app_common.h"
#include "app_buffer.h"
#include "app_pool.h"
#include "app_mqtt.h"
#include "app_msg.h"
#include "app_modbus.h"

typedef struct {
    bool is_running;
    app_buffer_handle download_buffer;
    app_buffer_handle upload_buffer;
} app_device_t;

extern app_device_t my_dev;

gate_state_t app_device_init(void);
void app_device_start(void);

#endif 
