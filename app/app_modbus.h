#ifndef __APP_MODBUS_H
#define __APP_MODBUS_H
#include "app_common.h"
#include "modbus/modbus.h"
gate_state_t app_modbus_init(void);
void app_modbus_deinit(void);
gate_state_t app_modbus_write_single_hold_register(uint8_t slave_adr, uint16_t data);
gate_state_t app_modbus_read_single_input_register(uint8_t slave_adr, uint16_t *data);
#endif
