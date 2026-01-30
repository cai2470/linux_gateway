
#include "app_buffer.h"
#include "pthread.h"

typedef struct
{
    char *ptr;    // 缓冲区指针
    int capacity; // 缓冲区容量
    int len;      // 表示已经存储的数据的长度
} app_sub_buffer_t;

typedef struct
{
    // 子缓冲区
    app_sub_buffer_t *sub_buffer[2];
    // 读写索引
    int read_index;
    int write_index;

    pthread_mutex_t read_mutex;  // 读锁: 读线程锁
    pthread_mutex_t write_mutex; // 写锁: 写线程锁

} app_buffer_t;

static app_sub_buffer_t *app_sub_buffer_init(int capacity)
{

    app_sub_buffer_t *sub_buffer = (app_sub_buffer_t *)malloc(sizeof(app_sub_buffer_t));

    if (sub_buffer == NULL)
    {
        log_error("子缓冲区初始化失败");
        return NULL;
    }
 
    sub_buffer->ptr = (char *)malloc(capacity);
    if (sub_buffer->ptr == NULL)
    {
        log_error("子缓冲区初始化失败");
        return NULL;
    }

    memset(sub_buffer->ptr, 0, capacity);

    sub_buffer->capacity = capacity;
    sub_buffer->len = 0;

    log_info("子缓冲区初始化成功");
    return sub_buffer;
}

app_buffer_handle app_buffer_init(int capacity)
{
    app_buffer_t *buffer = (app_buffer_t *)malloc(sizeof(app_buffer_t));
    if (buffer == NULL)
    {
        log_error("缓冲区初始化失败");
        return NULL;
    }

    buffer->sub_buffer[0] = app_sub_buffer_init(capacity);

    if (buffer->sub_buffer[0] == NULL)
    {
        return NULL;
    }

    buffer->sub_buffer[1] = app_sub_buffer_init(capacity);

    if (buffer->sub_buffer[1] == NULL)
    {
        return NULL;
    }

    buffer->read_index = 0;
    buffer->write_index = 1;

    pthread_mutex_init(&buffer->read_mutex, NULL);
    pthread_mutex_init(&buffer->write_mutex, NULL);

    return (app_buffer_handle)buffer;
}

void app_buffer_deinit(app_buffer_handle handle)
{
    app_buffer_t *buffer = (app_buffer_t *)handle;
    free(buffer->sub_buffer[0]->ptr);
    free(buffer->sub_buffer[0]);

    free(buffer->sub_buffer[1]->ptr);
    free(buffer->sub_buffer[1]);

    free(buffer);
}

gate_state_t app_buffer_write(app_buffer_handle handle, char *data, uint8_t len) // abcd =>> 1abcd
{
    app_buffer_t *buffer = (app_buffer_t *)handle;
    // 获取写锁
    pthread_mutex_lock(&buffer->write_mutex);

    app_sub_buffer_t *sub_buffer = buffer->sub_buffer[buffer->write_index];
    // 1. 判断空间释放满足
    if (sub_buffer->capacity - sub_buffer->len < len + 1)
    {
        log_error("缓冲区空间不足: 剩余 %d ,需要 %d", sub_buffer->capacity - sub_buffer->len, len + 1);
        pthread_mutex_unlock(&buffer->write_mutex);
        return GATE_ERROR;
    }

    // 2. 写数据
    // 2.1 写长度
    sub_buffer->ptr[sub_buffer->len] = len;
    // 2.2 写真正的数据
    memcpy(sub_buffer->ptr + (sub_buffer->len + 1), data, len);

    // 3. 更新长度
    sub_buffer->len += len + 1;

    // 释放写锁
    pthread_mutex_unlock(&buffer->write_mutex);
//    log_debug("缓冲区写入成功: %s", data);
    return GATE_OK;
}

gate_state_t app_buffer_read(app_buffer_handle handle,
                             char *data,
                             int capacity,
                             uint8_t *real_len)
{
    app_buffer_t *buffer = (app_buffer_t *)handle;
    // 获取读锁
    pthread_mutex_lock(&buffer->read_mutex);
    app_sub_buffer_t *r_buffer = buffer->sub_buffer[buffer->read_index];
    if (r_buffer->len == 0)
    {
        // 缓冲区为空
        //log_info("交换缓冲区");
        // 交换缓冲区: 交换索引
        // 先获取写锁
        pthread_mutex_lock(&buffer->write_mutex);
        int tmp = buffer->write_index;
        buffer->write_index = buffer->read_index;
        buffer->read_index = tmp;
        // 释放写锁
        pthread_mutex_unlock(&buffer->write_mutex);
        r_buffer = buffer->sub_buffer[buffer->read_index];
        if (r_buffer->len == 0)
        {
            //log_error("缓冲区为空");
            pthread_mutex_unlock(&buffer->read_mutex);
            return GATE_ERROR;
        }
    }
    // 缓冲区有数据
    // 1.  读取数据长度
    *real_len = r_buffer->ptr[0];
    if (*real_len > capacity)
    {
        log_error("缓冲区数据长度超出");
        pthread_mutex_unlock(&buffer->read_mutex);
        return GATE_ERROR;
    }
    // 2. 真正读数据
    memcpy(data, r_buffer->ptr + 1, *real_len);

    // 3. 移动后面的数据
    memmove(r_buffer->ptr, r_buffer->ptr + (1 + *real_len), r_buffer->len - (*real_len + 1));

    // 4. 更新长度
    r_buffer->len -= (*real_len + 1);

    // 释放读锁
    pthread_mutex_unlock(&buffer->read_mutex);

    return GATE_OK;
}
