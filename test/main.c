#include "hello.h"

int main(int argc, char const *argv[])
{
    say_hello();
    return 0;
}
/*

预处理:
    gcc -E hello.c -o hello.i 
    gcc -E main.c -o main.i
编译:
    gcc -S hello.i -o hello.s
    gcc -S main.i -o main.s 
汇编:
    gcc -c hello.s -o hello.o
    gcc -c main.s -o main.o   
链接:
    静态链接
        gcc --static main.o hello.o -o main1
    动态链接
        gcc main.o hello.o -o main


*/
