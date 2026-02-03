#include "app_runner.h"
#include "signal.h"

void sighandler(int sig)
{
    my_dev.is_running = false;
}

void app_runner_run(void)
{
    // 注册需要处理的信号:
    // ctrl+c: 2 和 终止: 15  kill + 进程id
    signal(SIGINT, sighandler);  // ctrl+c
    signal(SIGTERM, sighandler); // kill + 进程id


    // 1. 初始化设备
    gate_state_t state = app_device_init();
    if (state != GATE_OK)
    {
        log_info("设备初始化失败");
        return;
    }
    // 2. 启动设备
    app_device_start();

    while (my_dev.is_running)
    {
        log_info("设备运行中...");
        sleep(1);
    }

    // 释放资源
    app_device_deinit();

    log_info("设备已退出...");

}
