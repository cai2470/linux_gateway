#include "app_modbus.h"

modbus_t *rtx;
gate_state_t app_modbus_init(void)
{
    // 1. 创建 modbus rtu上下文
    rtx = modbus_new_rtu("/dev/ttyS1", 115200, 'N', 8, 1);
    if (rtx == NULL)
    {
        log_error("modbus rtu上下文 创建失败\n");
        return GATE_ERROR;
    }

    modbus_set_debug(rtx, true); // 打印调试信息. 实际生产环境要关闭
    
    // 增加响应超时时间到 1秒 (默认通常是0.5s，部分设备反应慢)
    modbus_set_response_timeout(rtx, 1, 0);

    // 2. 打开串口
    if (modbus_connect(rtx) == -1)
    {
        log_error("modbus rtu串口打开失败\n");
        modbus_free(rtx);
        return GATE_ERROR;
    }

    log_info("modbus rtc 初始化完成\n");
    return GATE_OK;
}

void app_modbus_deinit(void)
{
    modbus_close(rtx);
    modbus_free(rtx);
}

/*
设置电机速度:
    向保持寄存器写入电机的速度
    0


获取电机实际速度:
    输入寄存器
    0

*/
gate_state_t app_modbus_write_single_hold_register(uint8_t slave_adr, int addr, uint16_t data)
{
    // 设置从机地址
    modbus_set_slave(rtx, slave_adr);
    if (modbus_write_register(rtx, addr, data) == -1) // 写寄存器
    {
        log_error("modbus 写入失败\n");
        return GATE_ERROR;
    }
    log_info("modbus 写入成功\n");
    return GATE_OK;
}

gate_state_t app_modbus_read_single_input_register(uint8_t slave_adr, int addr, uint16_t *data)
{
    modbus_set_slave(rtx, slave_adr);
    if (modbus_read_input_registers(rtx, addr, 1, data) == -1)
    {
        log_error("modbus 读取失败\n");
        return GATE_ERROR;
    }
    log_info("modbus 读取成功\n");
    return GATE_OK;
}

gate_state_t app_modbus_write_single_coil(uint8_t slave_adr, int addr, int data)
{
    // 设置从机地址
    modbus_set_slave(rtx, slave_adr);
    if (modbus_write_bit(rtx, addr, data) == -1) // 写线圈 (Function Code 05)
    {
        log_error("modbus coil 写入失败\n");
        return GATE_ERROR;
    }
    log_info("modbus coil 写入成功\n");
    return GATE_OK;
}
