#include "app_device.h"

int main(int argc, char const *argv[])
{
    // 1. 初始化设备
    gate_state_t state = app_device_init();
    if (state != GATE_OK)
    {
        log_info("设备初始化失败");
        return -1;
    }
    // 2. 启动设备
    app_device_start();

    while (my_dev.is_running)
    {
        log_info("设备运行中...");
        sleep(1);
    }

    return 0;
}


/*

            cpu     操作系统
编译        x86     linux
运行        arm     linux

有任何一个不一致, 就需要使用交叉编译


*/
