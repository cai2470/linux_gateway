#include "app_common.h"
#include "pthread.h"

pthread_mutex_t mutex;
int num = 0;
void *thread_func(void *arg)
{

    for (size_t i = 0; i < 1000; i++)
    {
        // 加锁
        pthread_mutex_lock(&mutex);
        num++;  // 不是原子操作
        // 解锁
        pthread_mutex_unlock(&mutex);

        /*
            1. cpu从内存中读取num的值
            2. 进行+1
            3. 将结果写入内存
        */
    }
    
    return NULL;
}
int main(int argc, char const *argv[])
{
    // 初始化锁
    pthread_mutex_init(&mutex, NULL);
    pthread_t ids[20];
    for (size_t i = 0; i < 20; i++)
    {
        pthread_create(&ids[i], NULL, thread_func, NULL);
    }


    for (size_t i = 0; i < 20; i++)
    {
        pthread_join(ids[i], NULL);
    }
    log_info("num = %d\n", num);

    return 0;
}
