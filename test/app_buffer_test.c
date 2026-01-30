#include "app_buffer.h"

int main(int argc, char const *argv[])
{
    app_buffer_handle *buffer = app_buffer_init(16);

    app_buffer_write(buffer, "hello world", 11);
    app_buffer_write(buffer, "abc", 3);

    char data[1024];

    uint8_t real_len;
    app_buffer_read(buffer, data, sizeof(data), &real_len);
    printf("read: %.*s\n", real_len, data);

    app_buffer_read(buffer, data, sizeof(data), &real_len);
    printf("read: %.*s\n", real_len, data);

    app_buffer_read(buffer, data, sizeof(data), &real_len);
    printf("read: %.*s\n", real_len, data);
    return 0;
}
