#include "app_runner.h"
#include "ota.h"

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
        // log_info("ota");
        //ota_version_check();
        ota_auto_update();
    }

    else if (strcmp(argv[1], "daemon") == 0)
    {
        log_info("daemon");
    }
    else
    {
        log_info("参数错误: app | ota | daemon 必须是其中一个");
    }
    return 0;
}

/*

            cpu     操作系统
编译        x86     linux
运行        arm     linux

有任何一个不一致, 就需要使用交叉编译

---------------------------------------------
ota的流程:

1. 判断版本, 是否需要升级
    从服务读取最新的版本信息: 提供一个纯文本文件

2. 如果需要升级: 下载新的应用
    从服务下载新的应用, 二进程文件

3. 做校验: 做hash校验

4. 停止当前应用, 启用新的应用


*/
