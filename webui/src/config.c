#include "config.h"

int config_read(const char *config_path, app_config_t *apps, int *app_count) {
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        LOG_ERROR("无法打开配置文件: %s", config_path);
        return -1;
    }
    
    char line[BUFFER_SIZE];
    *app_count = 0;
    
    while (fgets(line, sizeof(line), fp) && *app_count < MAX_APPS) {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        // 解析配置行
        if (config_parse_line(line, &apps[*app_count])) {
            (*app_count)++;
        }
    }
    
    fclose(fp);
    LOG_INFO("读取配置: %d 个应用", *app_count);
    
    return 0;
}

int config_write(const char *config_path, app_config_t *apps, int app_count) {
    FILE *fp = fopen(config_path, "w");
    if (!fp) {
        LOG_ERROR("无法写入配置文件: %s", config_path);
        return -1;
    }
    
    // 写入头部注释
    fprintf(fp, "# 骁龙8Gen2小米13线程优化配置\n");
    fprintf(fp, "# 自动生成 - 请勿手动编辑\n");
    fprintf(fp, "# CPU 0-2: 小核 | 3-4: 中核 | 5-6: 大核 | 7: 超大核\n\n");
    
    // 写入配置
    for (int i = 0; i < app_count; i++) {
        char *line = config_format_line(&apps[i]);
        if (line) {
            fprintf(fp, "%s\n", line);
            free(line);
        }
    }
    
    fclose(fp);
    LOG_INFO("保存配置: %d 个应用", app_count);
    
    return 0;
}

bool config_parse_line(const char *line, app_config_t *app) {
    char buffer[BUFFER_SIZE];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    // 移除换行符
    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';
    newline = strchr(buffer, '\r');
    if (newline) *newline = '\0';
    
    // 解析格式: package=affinity 或 package{thread}=affinity
    char *equals = strchr(buffer, '=');
    if (!equals) return false;
    
    *equals = '\0';
    char *package_part = buffer;
    char *affinity = equals + 1;
    
    // 检查是否有线程名
    char *brace_open = strchr(package_part, '{');
    if (brace_open) {
        // 线程配置: package{thread}=affinity
        *brace_open = '\0';
        char *thread_name = brace_open + 1;
        char *brace_close = strchr(thread_name, '}');
        if (brace_close) *brace_close = '\0';
        
        // 查找或创建应用配置
        strncpy(app->package, package_part, sizeof(app->package) - 1);
        
        // 添加线程配置
        if (app->thread_count < MAX_THREADS) {
            strncpy(app->threads[app->thread_count].name, thread_name, 
                   sizeof(app->threads[app->thread_count].name) - 1);
            strncpy(app->threads[app->thread_count].affinity, affinity,
                   sizeof(app->threads[app->thread_count].affinity) - 1);
            app->thread_count++;
        }
    } else {
        // 主进程配置: package=affinity
        strncpy(app->package, package_part, sizeof(app->package) - 1);
        strncpy(app->affinity, affinity, sizeof(app->affinity) - 1);
        app->thread_count = 0;
    }
    
    return true;
}

char* config_format_line(const app_config_t *app) {
    char *line = (char*)malloc(BUFFER_SIZE);
    if (!line) return NULL;
    
    // 主进程配置
    snprintf(line, BUFFER_SIZE, "%s=%s", app->package, app->affinity);
    
    // TODO: 添加线程配置格式化
    
    return line;
}

app_config_t* config_find_app(app_config_t *apps, int app_count, const char *package) {
    for (int i = 0; i < app_count; i++) {
        if (strcmp(apps[i].package, package) == 0) {
            return &apps[i];
        }
    }
    return NULL;
}

int config_update_app(app_config_t *apps, int *app_count, const app_config_t *new_app) {
    // 查找现有配置
    app_config_t *existing = config_find_app(apps, *app_count, new_app->package);
    
    if (existing) {
        // 更新现有配置
        memcpy(existing, new_app, sizeof(app_config_t));
    } else {
        // 添加新配置
        if (*app_count >= MAX_APPS) {
            LOG_ERROR("配置数量已达上限");
            return -1;
        }
        memcpy(&apps[*app_count], new_app, sizeof(app_config_t));
        (*app_count)++;
    }
    
    return 0;
}
