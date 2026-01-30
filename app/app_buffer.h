#ifndef __APP_BUFFER_H
#define __APP_BUFFER_H
#include "app_common.h"

typedef void *app_buffer_handle;
// typedef app_buffer_t * app_buffer_handle;

app_buffer_handle app_buffer_init(int capacity);
void app_buffer_deinit(app_buffer_handle handle);
gate_state_t app_buffer_write(app_buffer_handle buffer, char *data, uint8_t len);
gate_state_t app_buffer_read(app_buffer_handle buffer, char *data, int capacity, uint8_t *real_len);
#endif
