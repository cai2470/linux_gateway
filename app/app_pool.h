#ifndef __APP_POOL_H
#define __APP_POOL_H
#include "app_common.h"
#include "pthread.h"
#include "mqueue.h"
gate_state_t app_pool_init(int init_size);
void app_pool_deinit(void);
gate_state_t app_pool_add_task(void (*task)(void *), void *args);
#endif
