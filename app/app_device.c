#include "app_device.h"

app_device_t my_dev;

gate_state_t app_device_init(void)
{
    // 1. 初始化mqtt
    gate_state_t err = app_mqtt_init();
    if (err != GATE_OK)
    {
        log_error("mqtt初始化失败 ");
        return GATE_ERROR;
    }
    log_info("mqtt初始化成功 ");
    // 2. 初始化线程池
    err = app_pool_init(10);
    if (err != GATE_OK)
    {
        log_error("线程池初始化失败 ");
        // 把mqtt反初始化
        app_mqtt_deinit();
        return GATE_ERROR;
    }
    log_info("线程池初始化成功 ");

    // 3. 初始化缓冲区
   my_dev.download_buffer = app_buffer_init(1024);
    if (my_dev.download_buffer == NULL)
    {
        log_error("下行缓冲区初始化失败 ");
        // 反初始化线程池
        app_pool_deinit(); // 去实现
        // 反初始化mqtt
        app_mqtt_deinit();
        return GATE_ERROR;
    }
    log_info("下行缓冲区初始化成功 ");
    my_dev.upload_buffer = app_buffer_init(1024);
    if (my_dev.upload_buffer == NULL)
    {
        log_error("上行缓冲区初始化失败 ");
        // 反初始化下行缓冲区
        app_buffer_deinit(my_dev.download_buffer); // 去实现
        // 反初始化线程池
        app_pool_deinit(); // 去实现
        // 反初始化mqtt
        app_mqtt_deinit();
        return GATE_ERROR;
    }
    log_info("上行缓冲区初始化成功 ");

    // 4. 初始化modbus模块
}



void mqtt_recv_cb(char *data, int len);
void upload_task(void *);
void modbus_task(void *);
void app_device_start(void)
{
    // 1. 注册mqtt接收到数据的回调函数
    app_mqtt_register_recv_cb(mqtt_recv_cb);

    // 2. 添加上行任务
    app_pool_add_task(upload_task, NULL);
    app_pool_add_task(upload_task, NULL);

    // 3. 添加modbus读写任务
    app_pool_add_task(modbus_task, NULL);
    app_pool_add_task(modbus_task, NULL);

    my_dev.is_running = true;
}

void mqtt_recv_cb(char *data, int len)
{
    log_info("收到mqtt数据 %.*s", len, data);

    app_buffer_write(my_dev.download_buffer, data, len);
}

void upload_task(void *args)
{
    char data[1024];
    uint8_t real_len = 0;
    while (1)
    {
        gate_state_t err = app_buffer_read(my_dev.upload_buffer, data, sizeof(data), &real_len);
        if (err == GATE_OK)
        {
            data[real_len] = '\0';
            log_info("上传数据 %.*s", real_len, data);
            app_mqtt_send(data, real_len);
        }
    }
}

void modbus_task(void *args)
{
    char data[1024];
    uint8_t real_len = 0;
    while (1)
    {
        // 从下行缓冲区读取数据
        gate_state_t err = app_buffer_read(my_dev.download_buffer, data, sizeof(data), &real_len);
        if (err != GATE_OK)
        {
            continue;
        }

        // 打印读到的数据
        log_info("modubs读写线程: %.*s", real_len, data);
    }
}
