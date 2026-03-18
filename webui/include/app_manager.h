#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include "common.h"

// 获取已安装应用列表
int app_get_installed_list(app_config_t *apps, int max_count);

// 获取应用名称
char* app_get_name(const char *package);

// 获取应用图标(base64)
char* app_get_icon(const char *package);

// 检查应用是否运行
bool app_is_running(const char *package);

// 获取应用PID
int app_get_pid(const char *package);

// 获取进程CPU亲和性
char* app_get_affinity(int pid);

// 获取系统状态
int system_get_status(system_status_t *status);

// 获取CPU核心信息
int system_get_cpu_info(cpu_core_t *cores, int max_count);

// 检查AppOpt是否运行
bool system_is_appopt_running(void);

// 执行shell命令并获取输出
char* exec_command(const char *cmd);

#endif // APP_MANAGER_H
