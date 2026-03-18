#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

// 读取配置文件
int config_read(const char *config_path, app_config_t *apps, int *app_count);

// 写入配置文件
int config_write(const char *config_path, app_config_t *apps, int app_count);

// 解析配置行
bool config_parse_line(const char *line, app_config_t *app);

// 格式化配置行
char* config_format_line(const app_config_t *app);

// 查找应用配置
app_config_t* config_find_app(app_config_t *apps, int app_count, const char *package);

// 添加或更新应用配置
int config_update_app(app_config_t *apps, int *app_count, const app_config_t *new_app);

#endif // CONFIG_H
