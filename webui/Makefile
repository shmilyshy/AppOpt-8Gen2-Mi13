# AppOpt WebUI Makefile
# 作者: 刘十三

# 编译器配置
CC = gcc
AR = ar
OBJCOPY = objcopy

# Android NDK配置 (需要设置NDK_PATH环境变量)
NDK_PATH ?= $(HOME)/Android/Sdk/ndk/25.2.9519653
ANDROID_API = 30
ANDROID_ARCH = aarch64-linux-android

# 交叉编译工具链
ifeq ($(ARCH),arm64)
    CC = $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/$(ANDROID_ARCH)$(ANDROID_API)-clang
    AR = $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-ar
    OBJCOPY = $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-objcopy
endif

# 目录配置
SRC_DIR = src
INC_DIR = include
WEB_DIR = web
BUILD_DIR = build
BIN_DIR = bin

# 源文件
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# 目标文件
TARGET = $(BIN_DIR)/AppOptWebUI

# 编译选项
CFLAGS = -I$(INC_DIR) -Wall -O2 -std=c11

# 依赖库路径 (可通过环境变量覆盖)
MICROHTTPD_INCLUDE ?= /usr/local/include
MICROHTTPD_LIB ?= /usr/local/lib
CJSON_INCLUDE ?= /usr/local/include
CJSON_LIB ?= /usr/local/lib

# 添加依赖库路径
CFLAGS += -I$(MICROHTTPD_INCLUDE) -I$(CJSON_INCLUDE)

# 静态链接选项 (用于Android)
ifeq ($(ARCH),arm64)
    LDFLAGS = -L$(MICROHTTPD_LIB) -L$(CJSON_LIB) -static -lmicrohttpd -lcjson
    OBJCOPY_ARCH = aarch64
    OBJCOPY_FORMAT = elf64-littleaarch64
else
    LDFLAGS = -L$(MICROHTTPD_LIB) -L$(CJSON_LIB) -lmicrohttpd -lcjson -lpthread
    OBJCOPY_ARCH = i386:x86-64
    OBJCOPY_FORMAT = elf64-x86-64
endif

# 默认目标
all: $(TARGET)

# 创建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# 编译C源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 链接生成可执行文件
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "编译完成: $@"
	@ls -lh $@

# 清理
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "清理完成"

# 安装到设备
install: $(TARGET)
	adb push $(TARGET) /data/local/tmp/
	adb shell "su -c 'chmod 755 /data/local/tmp/AppOptWebUI'"
	@echo "已安装到设备 /data/local/tmp/AppOptWebUI"

# 运行测试
test: install
	adb shell "su -c '/data/local/tmp/AppOptWebUI'"

# 帮助信息
help:
	@echo "AppOpt WebUI 编译系统"
	@echo ""
	@echo "使用方法:"
	@echo "  make              - 编译本地版本"
	@echo "  make ARCH=arm64   - 交叉编译Android ARM64版本"
	@echo "  make clean        - 清理编译文件"
	@echo "  make install      - 安装到Android设备"
	@echo "  make test         - 安装并运行测试"
	@echo ""
	@echo "环境变量:"
	@echo "  NDK_PATH          - Android NDK路径"
	@echo ""
	@echo "示例:"
	@echo "  export NDK_PATH=/path/to/ndk"
	@echo "  make ARCH=arm64"

.PHONY: all clean install test help
