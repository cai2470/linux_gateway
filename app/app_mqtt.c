#include "app_mqtt.h"

#define MQTT_SERVER_URL ""
#define CLIENTID "my_client"

#define PULL_TOPIC "pull"
#define PUSH_TOPIC "push"

typedef struct
{
    MQTTClient client;
} app_mqtt_t;

static app_mqtt_t my_mqtt;

void conn_lost(void *context, char *cause);
int msg_recv(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void delivery_complete(void *context, MQTTClient_deliveryToken dt);

gate_state_t app_mqtt_init(void)
{
    // 1. Create MQTTClient 创建mqtt客户端
    if (MQTTClient_create(&my_mqtt.client,
                          MQTT_SERVER_URL,
                          CLIENTID,
                          MQTTCLIENT_PERSISTENCE_NONE,
                          NULL) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_create failed");
        return GATE_ERROR;
    }
    log_info("MQTTClient_create success");

    // 2. 设置回调函数
    MQTTClient_setCallbacks(my_mqtt.client, NULL, conn_lost, msg_recv, delivery_complete);

    // 3. 创建连接
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    if (MQTTClient_connect(my_mqtt.client, &conn_opts) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_connect failed");
        return GATE_ERROR;
    }
    log_info("MQTTClient_connect success");

    // 4. 订阅主题
    if (MQTTClient_subscribe(my_mqtt.client, PULL_TOPIC, 0) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_subscribe failed");
        return GATE_ERROR;
    }
    log_info("MQTTClient_subscribe success");
}

void conn_lost(void *context, char *cause)
{
    log_error("MQTT connection lost: %s", cause);

    // 重新连接
}
int msg_recv(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    log_info("MQTT message received: %.*s", message->payloadlen, message->payload);


    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void delivery_complete(void *context, MQTTClient_deliveryToken dt)
{
    log_info("MQTTClient_deliveryToken %d", dt);
}
