#ifndef API_H
#define API_H

#include "common.h"
#include <microhttpd.h>

// API路由处理函数
enum MHD_Result api_handle_request(struct MHD_Connection *connection,
                       const char *url,
                       const char *method,
                       const char *upload_data,
                       size_t *upload_data_size);

// GET /api/apps - 获取应用列表
char* api_get_apps(void);

// GET /api/config - 获取配置
char* api_get_config(void);

// POST /api/save - 保存配置
char* api_save_config(const char *json_data);

// GET /api/status - 获取系统状态
char* api_get_status(void);

// GET /api/presets - 获取预设方案
char* api_get_presets(void);

// GET /api/installed_apps - 获取所有已安装应用
char* api_get_installed_apps(void);

// POST /api/apply_preset - 应用预设方案
char* api_apply_preset(const char *json_data);

#endif // API_H
