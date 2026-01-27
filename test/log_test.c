#include "log/log.h"

int main(int argc, char const *argv[])
{
    // 设置日志级别
    log_set_level(LOG_INFO);

    log_trace("hello world");
    log_debug("hello world");
    log_info("hello world");
    log_warn("hello world");
    log_error("hello world");
    log_fatal("hello world");
    return 0;
}
/*
LOG_TRACE,
LOG_DEBUG,
LOG_INFO,
LOG_WARN,
LOG_ERROR,
LOG_FATAL


*/
