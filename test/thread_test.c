#include "app_common.h"
#include "pthread.h"

void *thread_func(void *arg)
{
    log_info("子线程 thread id: %d\n", gettid());
    return NULL;
}
int main(int argc, char const *argv[])
{
    // 获取当前线程的id
    pid_t id = gettid();
    log_info("主线程 thread id: %d\n", id);

    pthread_t tid;
    int r = pthread_create(&tid, NULL, thread_func, NULL);
    if (r != 0)
    {
        log_info("创建线程失败\n");
        return -1;
    }
    log_info("创建线程成功\n");

    //sleep(10);
    // 阻塞当前线程，等待子线程tid结束, 然后函数才会返回
    pthread_join(tid, NULL);  // 线程礼让
    return 0;
}
