#include "app_pool.h"

typedef struct
{
    void (*task)(void *);
    void *args;
} task_t;

typedef struct
{
    int size;
    pthread_t *pids;
    mqd_t mq_id;
    char *mq_name;
} app_pool_t;

static app_pool_t my_pool;

void *thead_fun(void *);
gate_state_t app_pool_init(int init_size)
{
    my_pool.size = init_size;
    my_pool.mq_name = "/my_pool";
    // 1. 先创建一个消息队列
    struct mq_attr attr;
    attr.mq_maxmsg = init_size;
    attr.mq_msgsize = sizeof(task_t);
    attr.mq_flags = 0;
    my_pool.mq_id = mq_open(my_pool.mq_name, O_CREAT | O_RDWR, 0666, &attr);

    if (my_pool.mq_id == -1)
    {
        log_error("线程池需要的消息队列创建失败");
        return GATE_ERROR;
    }
    log_info("线程池需要的消息队列创建成功");

    // 2. 再去创建init_size多个线程
    my_pool.pids = (pthread_t *)malloc(init_size * sizeof(pthread_t));
    for (int i = 0; i < init_size; i++)
    {
        pthread_create(&my_pool.pids[i], NULL, thead_fun, NULL);
    }
    return GATE_OK;
}

void app_pool_deinit(void)
{
    // 关闭线程
    for (int i = 0; i < my_pool.size; i++)
    {
        // pthread_cancel() 在线程外部取消其他线程
        //  pthread_exit()  在线程内部退出当前线程
        pthread_cancel(my_pool.pids[i]);
    }

    // 释放保存线程id的内存
    free(my_pool.pids);

    // 关闭和释放消息队列
    mq_close(my_pool.mq_id);
    mq_unlink(my_pool.mq_name);
}

void *thead_fun(void *args)
{
    while (1)
    {
        // 1. 从消息队列获取消息   1. void fun(void *)  2. 函数的参数
        task_t task;
        ssize_t real_size = mq_receive(my_pool.mq_id, &task, sizeof(task_t), NULL);
        if (real_size > 0 && task.task)
        {
            task.task(task.args);
        }
    }
}

gate_state_t app_pool_add_task(void (*task)(void *), void *args)
{
    task_t tmp_task = {
        .task = task,
        .args = args};
    // 给消息队列发送消息
    int r = mq_send(my_pool.mq_id, (char *)&tmp_task, sizeof(task_t), 0);
    if (r == -1)
    {
        log_error("任务添加失败");
        return GATE_ERROR;
    }
    log_info("任务添加成功");
    return GATE_OK;
}
