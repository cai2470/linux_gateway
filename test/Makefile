cc := gcc  # 定义变量

# 变量拼接
a := hello.o
a += main.o

main: main.o hello.o
	-@$(cc) $^ -o main
	-afasdf
	-@./main

hello.o: hello.c hello.h
	@$(cc) -c $< -o hello.o

main.o: main.c hello.h
	@$(cc) -c main.c -o $@

# 定义伪目标:  当目标不对应具体的文件的时候, 把目标定义成伪目标
.PHONY: clear
clear: 
	rm -f *.o main


# make的特殊变量

# $@: 表示目标
# $^: 表示所有依赖 (去重)
# $<: 表示第一个依赖

# @ 静默执行
# - 忽略错误
