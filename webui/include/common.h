#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// 版本信息
#define WEBUI_VERSION "1.0.0"
#define WEBUI_PORT 8080

// 路径定义
#define MODULE_PATH "/data/adb/modules/AppOpt_8Gen2_Mi13"
#define CONFIG_FILE MODULE_PATH "/applist.conf"
#define LOG_FILE "/data/local/tmp/appopt_webui.log"

// 缓冲区大小
#define BUFFER_SIZE 4096
#define MAX_APPS 512
#define MAX_THREADS 64
#define MAX_PACKAGE_NAME 256
#define MAX_APP_NAME 128

// 日志宏
#define LOG_INFO(fmt, ...) fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)

// CPU核心类型
typedef enum {
    CPU_TYPE_SMALL = 0,   // 小核 0-2
    CPU_TYPE_MID = 1,     // 中核 3-4
    CPU_TYPE_BIG = 2,     // 大核 5-6
    CPU_TYPE_PRIME = 3    // 超大核 7
} cpu_type_t;

// CPU核心信息
typedef struct {
    int id;
    cpu_type_t type;
    int min_freq;
    int max_freq;
    bool online;
} cpu_core_t;

// 线程配置
typedef struct {
    char name[64];
    char affinity[32];
} thread_config_t;

// 应用配置
typedef struct {
    char package[MAX_PACKAGE_NAME];
    char name[MAX_APP_NAME];
    char affinity[32];
    thread_config_t threads[MAX_THREADS];
    int thread_count;
    bool running;
    int pid;
} app_config_t;

// 系统状态
typedef struct {
    char device_model[64];
    char soc_model[32];
    cpu_core_t cores[8];
    int core_count;
    bool appopt_running;
    int appopt_pid;
} system_status_t;

#endif // COMMON_H
