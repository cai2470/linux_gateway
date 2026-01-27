#include "app_mqtt.h"

void mqtt_recv_data(char *data, int len)
{
    log_info("recv data: %s", data);
}
int main(int argc, char const *argv[])
{
    app_mqtt_init();

    app_mqtt_register_recv_cb(mqtt_recv_data);

    app_mqtt_send("hello world", 11);

    app_mqtt_send("hello world1", 12);

    sleep(1000000);
    return 0;
}
