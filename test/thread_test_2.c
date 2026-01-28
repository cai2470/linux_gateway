#include "app_common.h"
#include "pthread.h"
#include "mqueue.h"

void *thread_func(void *arg)
{
    struct mq_attr attr;
    attr.mq_flags = 0;      // 0: 阻塞式读取数据
    attr.mq_maxmsg = 10;    // 最大消息数
    attr.mq_msgsize = 1024; // 每条消息最大字节数
    mqd_t mq = mq_open("/my_mq", O_RDWR | O_CREAT, 0666, &attr);

    char buf[1024];
    memset(buf, 0, 1024);
    int i = 0;
    while (1)
    {
        i++;
        ssize_t real_size = mq_receive(mq, buf, 1024, NULL);
        printf("thread_func: %.*s\n", real_size, buf);

        if(i == 2){
            break;
        }
    }

    mq_close(mq);

    return NULL;
}
int main(int argc, char const *argv[])
{

    // 创建消息队列
    struct mq_attr attr;
    attr.mq_flags = 0;      // 0: 阻塞式读取数据
    attr.mq_maxmsg = 10;    // 最大消息数
    attr.mq_msgsize = 1024; // 每条消息最大字节数
    mqd_t mq = mq_open("/my_mq", O_RDWR | O_CREAT, 0666, &attr);

    // 向消息队列写数据
    char *str = "hello world";
    mq_send(mq, str, strlen(str), 0);

    str = "hello world hello";
    mq_send(mq, str, strlen(str), 0);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_func, NULL);

    // 等待子线程结束
    pthread_join(tid, NULL);

    // 关闭消息队列
    mq_close(mq);

    // 删除消息队列
    mq_unlink("/my_mq");

    return 0;
}
