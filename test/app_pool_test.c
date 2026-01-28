#include "app_pool.h"

void task(void *args)
{
    log_info("task %d\n", *(int *)args);
}

int main(int argc, char const *argv[])
{
    gate_state_t state = app_pool_init(10);
    if (state != GATE_OK)
    {
        log_error("app_pool_init error\n");
    }
    log_info("app_pool_init ok\n");

    for (size_t i = 0; i < 20; i++)
    {
        int *j = malloc(sizeof(int));
        *j = i;
        app_pool_add_task(task, j);
    }

    sleep(1000);

    return 0;
}
