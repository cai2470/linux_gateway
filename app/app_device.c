#include "app_device.h"


// --- 宏定义 (根据你的电机板 Common_config.h) ---
#define ADDR_COIL_START      0  // 启动/停止 (线圈)
#define ADDR_HOLD_ANGLE      0  // 目标角度 (保持寄存器, 占2个: 0,1)
#define ADDR_HOLD_SPEED      2  // 目标速度 (保持寄存器, 占2个: 2,3)
#define ADDR_INPUT_SPEED     0  // 当前速度 (输入寄存器, 占2个: 0,1)

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
    err = app_modbus_init();
    if (err != GATE_OK)
    {
        log_error("modbus模块初始化失败 ");
    }
    log_info("modbus模块初始化成功 ");

    log_info("app_device 模块初始化成功 ");
    return GATE_OK;
}

void app_device_deinit(void)
{
    app_mqtt_deinit();
    app_modbus_deinit();
    app_pool_deinit();
    app_buffer_deinit(my_dev.download_buffer);
    app_buffer_deinit(my_dev.upload_buffer);
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
        if (err != GATE_OK)
        {
            usleep(10000); // 缓冲区为空时休眠10ms
            continue;
        }
        log_info("上传数据 %.*s", real_len, data);
        app_mqtt_send(data, real_len);
    }
}

void modbus_task(void *args)
{
    char data[1024];
    uint8_t real_len = 0;

    while (1)
    {
        // 1. 从下行缓冲区读取数据
        // 【修正】去掉 (int*) 强转，因为 real_len 本身就是 uint8_t
        gate_state_t err = app_buffer_read(my_dev.download_buffer, data, sizeof(data), &real_len);
        if (err != GATE_OK)
        {
            usleep(10000); // 缓冲区为空时休眠10ms
            continue;
        }

        data[real_len] = '\0'; // 补上结束符，确保 cJSON 解析安全
        // 2. 打印读到的数据
        log_info("modbus任务读取: %.*s", real_len, data);
        
        app_msg_t msg;
        // 初始化结构体，防止野指针
        memset(&msg, 0, sizeof(app_msg_t)); 
        
        // 解析 JSON
        app_msg_json_2_msg(data, &msg);

        // 3. 业务逻辑处理
        if (msg.connectType && (strcmp(msg.connectType, "rs485") == 0 || strcmp(msg.connectType, "modbus") == 0))
        {
            // ---------------- 处理 SET (写入控制) ----------------
            if (msg.type && strcmp(msg.type, "set") == 0)
            {
                // 建议顺序：先写参数，最后写启动信号
                if (msg.is_start == 1)
                {
                    // (1) 设置目标速度 -> 保持寄存器 地址 2 (Float 拆分为 2个 uint16)
                    float speed_f = (float)msg.targetSpeed;
                    uint32_t speed_u32;
                    // 使用 memcpy 避免类型转换带来的精度丢失
                    memcpy(&speed_u32, &speed_f, sizeof(uint32_t));
                    
                    // 拆分高低位
                    uint16_t speed_high = (uint16_t)((speed_u32 >> 16) & 0xFFFF); // 高16位
                    uint16_t speed_low  = (uint16_t)(speed_u32 & 0xFFFF);         // 低16位

                    // STM32 float存储通常是：低地址存低位，高地址存高位 (Little Endian)
                    // 所以：寄存器[2] = Low, 寄存器[3] = High
                    
                    // 写入低 16 位 -> 寄存器 2
                    err = app_modbus_write_single_hold_register(msg.id, ADDR_HOLD_SPEED, speed_low);
                    if(err != GATE_OK) log_error("Set Speed Low Failed");
                    
                    // 写入高 16 位 -> 寄存器 3
                    err = app_modbus_write_single_hold_register(msg.id, ADDR_HOLD_SPEED + 1, speed_high);
                    if(err != GATE_OK) log_error("Set Speed High Failed");

                    usleep(50000); // 延时 50ms 防止指令冲突

                    // (2) 设置目标角度 -> 保持寄存器 地址 0 (逻辑同上)
                    if (msg.targetAngle != 0) {
                        float angle_f = (float)msg.targetAngle;
                        uint32_t angle_u32;
                        memcpy(&angle_u32, &angle_f, sizeof(uint32_t));
                        uint16_t angle_high = (uint16_t)((angle_u32 >> 16) & 0xFFFF);
                        uint16_t angle_low  = (uint16_t)(angle_u32 & 0xFFFF);

                        app_modbus_write_single_hold_register(msg.id, ADDR_HOLD_ANGLE, angle_low);
                        app_modbus_write_single_hold_register(msg.id, ADDR_HOLD_ANGLE + 1, angle_high);
                        usleep(50000);
                    }

                    // (3) 发送启动命令 -> 线圈 地址 0
                    // 【关键】必须使用写线圈函数 (功能码 05)
                    err = app_modbus_write_single_coil(msg.id, ADDR_COIL_START, 1); 
                    if(err != GATE_OK) {
                        log_error("Failed to start motor");
                    } else {
                        log_info("Motor Start Command Sent");
                    }

                    // (4) 回传 JSON 响应给 MQTT
                    log_info("modbus写入完成: id=%d speed=%d", msg.id, msg.targetSpeed);
                    app_msg_msg_2_json(&msg, data);
                    app_buffer_write(my_dev.upload_buffer, data, strlen(data));
                }
                else 
                {
                    // 处理停止逻辑 (is_start = 0)
                    app_modbus_write_single_coil(msg.id, ADDR_COIL_START, 0);
                    log_info("Motor Stop Command Sent");
                }
            }
            // ---------------- 处理 GET (查询状态) ----------------
            else if (msg.type && strcmp(msg.type, "get") == 0)
            {
                // 读取 float 类型的速度需要读 2 个寄存器
                uint16_t speed_low = 0, speed_high = 0;
                
                // 读低位 (Reg 0)
                app_modbus_read_single_input_register(msg.id, ADDR_INPUT_SPEED, &speed_low);
                // 读高位 (Reg 1)
                app_modbus_read_single_input_register(msg.id, ADDR_INPUT_SPEED + 1, &speed_high);
                
                // 组合回 float
                uint32_t temp_u32 = ((uint32_t)speed_high << 16) | speed_low;
                float temp_speed_f;
                memcpy(&temp_speed_f, &temp_u32, sizeof(float));

                msg.targetSpeed = (int)temp_speed_f; // 转回 int 用于显示
                
                log_info("查询成功, 当前速度: %d (Hex: %04X %04X)", msg.targetSpeed, speed_high, speed_low);

                // 回传数据
                app_msg_msg_2_json(&msg, data);
                app_buffer_write(my_dev.upload_buffer, data, strlen(data));
            }
        }
        else if (msg.connectType && strcmp(msg.connectType, "lora") == 0)
        {
            // Lora 逻辑保留
        }

        // 4. 内存释放 (非常重要，防止内存泄漏)
        if (msg.connectType) free(msg.connectType);
        if (msg.type) free(msg.type);
    }
}

/*
设置电机速度:
    {
    "id": 5,
    "connectType": "modbus",
    "type":"set",
    "is_start": 1,
    "targetAngle": 3600,
    "targetSpeed": 1600
}


*/
