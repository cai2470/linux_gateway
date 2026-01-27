#ifndef __MQTT_APP_H
#define __MQTT_APP_H
#include "app_common.h"

#include "MQTTClient.h"

gate_state_t app_mqtt_init(void);
void app_mqtt_deinit(void);
gate_state_t app_mqtt_send(char *data, int len);
void app_mqtt_register_recv_cb(void (*cb)(char *data, int len));
#endif
