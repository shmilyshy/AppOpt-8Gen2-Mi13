#!/bin/bash
# AppOpt WebUI 编译脚本
# 作者: 刘十三

set -e

echo "========================================="
echo "AppOpt WebUI 编译脚本"
echo "作者: 刘十三"
echo "========================================="
echo ""

# 检查NDK路径
if [ -z "$NDK_PATH" ]; then
    echo "错误: 未设置 NDK_PATH 环境变量"
    echo "请设置: export NDK_PATH=/path/to/android-ndk"
    exit 1
fi

if [ ! -d "$NDK_PATH" ]; then
    echo "错误: NDK路径不存在: $NDK_PATH"
    exit 1
fi

echo "✓ NDK路径: $NDK_PATH"
echo ""

# 检查依赖库
echo "检查依赖库..."
echo "  - libmicrohttpd (HTTP服务器)"
echo "  - cJSON (JSON解析)"
echo ""

# 编译
echo "开始编译..."
make clean
make ARCH=arm64

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "✓ 编译成功!"
    echo "========================================="
    echo ""
    echo "输出文件: bin/AppOptWebUI"
    ls -lh bin/AppOptWebUI
    echo ""
    echo "安装到设备:"
    echo "  make install"
    echo ""
    echo "或手动安装:"
    echo "  adb push bin/AppOptWebUI /data/adb/modules/AppOpt_8Gen2_Mi13/"
    echo "  adb shell su -c 'chmod 755 /data/adb/modules/AppOpt_8Gen2_Mi13/AppOptWebUI'"
    echo ""
else
    echo ""
    echo "========================================="
    echo "✗ 编译失败"
    echo "========================================="
    exit 1
fi
