#include "app_runner.h"

int main(int argc, char const *argv[])
{
    // argv[0] 是程序名(gateway) ...
    if (argc == 1)
    {
        log_info("请传入一个参数: app | ota | daemon");
    }
    else if (strcmp(argv[1], "app") == 0)
    {
        app_runner_run();
    }
    else if (strcmp(argv[1], "ota") == 0)
    {
        log_info("ota");
    }
    else if (strcmp(argv[1], "daemon") == 0)
    {
        log_info("daemon");
    }
    else{
        log_info("参数错误: app | ota | daemon 必须是其中一个");
    }
    return 0;
}

/*

            cpu     操作系统
编译        x86     linux
运行        arm     linux

有任何一个不一致, 就需要使用交叉编译


*/
