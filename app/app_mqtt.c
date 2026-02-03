#include "app_mqtt.h"

#define MQTT_SERVER_URL "tcp://192.168.54.24:1883"
#define CLIENTID "my_client"

#define PULL_TOPIC "pull"
#define PUSH_TOPIC "push"

typedef struct
{
    MQTTClient client;
    void (*recv_cb)(char *data, int len);
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
    return GATE_OK;
}


void app_mqtt_deinit(void)
{
    // 取消订阅
    MQTTClient_unsubscribe(my_mqtt.client, PULL_TOPIC);
    // 断开连接
    MQTTClient_disconnect(my_mqtt.client, 5000);
    // 销毁客户端
    MQTTClient_destroy(&my_mqtt.client);
}

void conn_lost(void *context, char *cause)
{
    log_error("MQTT connection lost: %s", cause);
    while (1)
    {
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        if (MQTTClient_connect(my_mqtt.client, &conn_opts) != MQTTCLIENT_SUCCESS)
        {
            log_error("MQTTClient_connect failed");
            sleep(1);
            continue;
        }
        log_info("MQTTClient_connect success");
        break;
    }

    while (1)
    {
        if (MQTTClient_subscribe(my_mqtt.client, PULL_TOPIC, 0) != MQTTCLIENT_SUCCESS)
        {
            log_error("MQTTClient_subscribe failed");
            sleep(1);
            continue;
        }
        log_info("MQTTClient_subscribe success");
        break;
    }
}
int msg_recv(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    log_info("MQTT message received: %.*s", message->payloadlen, (char *)message->payload);
    if (my_mqtt.recv_cb)
    {
        my_mqtt.recv_cb(message->payload, message->payloadlen);
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void delivery_complete(void *context, MQTTClient_deliveryToken dt)
{
    log_info("MQTTClient_deliveryToken %d", dt);
}

gate_state_t app_mqtt_send(char *data, int len)
{
    if (MQTTClient_publish(my_mqtt.client,
                           PUSH_TOPIC,
                           len,
                           data,
                           0,
                           0,
                           NULL) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_publish error");
        return GATE_ERROR;
    }
    log_info("MQTTClient_publish success");
    return GATE_OK;
}

void app_mqtt_register_recv_cb(void (*cb)(char *data, int len))
{
    my_mqtt.recv_cb = cb;
}
