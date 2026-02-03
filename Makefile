CC:=$(CROSS_COMPILE)gcc

BOARD_DIR := $(shell pwd)/
#改为自己开发板的ip地址
PEER := root@192.168.54.58

#CFLAGS += -Wall -Wextra

CFLAGS += -I.
CFLAGS += -Ithirdparty
CFLAGS += -Iapp
CFLAGS += -Idaemon
CFLAGS += -Iota

ifdef SYSROOT
	CFLAGS += --sysroot=$(SYSROOT)
endif

LDLIBS += -lpaho-mqtt3c
LDLIBS += -lcurl
LDLIBS += -lcrypto
LDLIBS += -lmodbus
#modbus文件路径,toolchain前面的路径需要改为自己的工程路径
#LDLIBS += -L/home/shtos/桌面/01_Projects/485_gateway/toolchain/arm-linux-gnueabihf/lib

SRC += $(shell find app -name "*.c" -type f)
SRC += $(shell find daemon -name "*.c"  -type f)
SRC += $(shell find ota -name "*.c" -type f)
SRC += $(shell find thirdparty -name "*.c"  -type f)

OBJ := $(SRC:.c=.o)

TARGET := gateway

.PHONY: all, clean

all: $(TARGET)

clean:
	@-rm -f $(TARGET) $(OBJ) main.o
$(TARGET): main.o $(OBJ)
	@-$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

cross-compile:
	@CROSS_COMPILE=$(BOARD_DIR)/toolchain/bin/arm-linux-gnueabihf- \
	 SYSROOT=$(BOARD_DIR)/sysroot \
	 make -j16
	@scp -O $(TARGET) $(PEER):/usr/bin/$(TARGET)

cross-init:
	@scp -O init/S99gateway $(PEER):/etc/init.d/S99gateway

%.o: %.c
	@-$(CC) $(CFLAGS) -c $^ -o $@

%_test: test/%_test.o $(OBJ)
	@-$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
	@-./$@
	@-rm $@ $^

#./表示让系统在当前目录(GATEWAY_SHT)下查找这个程序
# $^表示所有依赖文件，即所有的.o文件
# $@表示目标(target)文件，即可执行文件
# -I表示在编译时添加头文件搜索路径

