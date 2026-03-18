#include "app_manager.h"

char* exec_command(const char *cmd) {
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        LOG_ERROR("执行命令失败: %s", cmd);
        return NULL;
    }
    
    char *result = (char*)malloc(BUFFER_SIZE);
    if (!result) {
        pclose(fp);
        return NULL;
    }
    
    size_t total = 0;
    size_t read_size;
    char buffer[1024];
    
    while ((read_size = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (total + read_size >= BUFFER_SIZE - 1) break;
        memcpy(result + total, buffer, read_size);
        total += read_size;
    }
    result[total] = '\0';
    
    pclose(fp);
    return result;
}

int app_get_installed_list(app_config_t *apps, int max_count) {
    char *output = exec_command("pm list packages -3");
    if (!output) return 0;
    
    int count = 0;
    char *line = strtok(output, "\n");
    
    while (line && count < max_count) {
        // 格式: package:com.example.app
        if (strncmp(line, "package:", 8) == 0) {
            char *package = line + 8;
            strncpy(apps[count].package, package, sizeof(apps[count].package) - 1);
            
            // 获取应用名称
            char *name = app_get_name(package);
            if (name) {
                strncpy(apps[count].name, name, sizeof(apps[count].name) - 1);
                free(name);
            } else {
                strncpy(apps[count].name, package, sizeof(apps[count].name) - 1);
            }
            
            // 检查运行状态
            apps[count].running = app_is_running(package);
            apps[count].pid = app_get_pid(package);
            
            count++;
        }
        line = strtok(NULL, "\n");
    }
    
    free(output);
    return count;
}

char* app_get_name(const char *package) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), 
             "pm dump %s | grep -A1 'labelRes' | tail -1 | sed 's/.*=//g'", 
             package);
    
    char *output = exec_command(cmd);
    if (output && strlen(output) > 0) {
        // 移除换行符
        char *newline = strchr(output, '\n');
        if (newline) *newline = '\0';
        return output;
    }
    
    if (output) free(output);
    return strdup(package);
}

char* app_get_icon(const char *package) {
    // TODO: 实现图标获取和base64编码
    return strdup("");
}

bool app_is_running(const char *package) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "pidof %s", package);
    
    char *output = exec_command(cmd);
    bool running = (output && strlen(output) > 0);
    
    if (output) free(output);
    return running;
}

int app_get_pid(const char *package) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "pidof %s", package);
    
    char *output = exec_command(cmd);
    int pid = 0;
    
    if (output && strlen(output) > 0) {
        pid = atoi(output);
    }
    
    if (output) free(output);
    return pid;
}

char* app_get_affinity(int pid) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "cat /proc/%d/status | grep Cpus_allowed_list", pid);
    
    char *output = exec_command(cmd);
    if (output) {
        // 格式: Cpus_allowed_list:  0-2
        char *colon = strchr(output, ':');
        if (colon) {
            char *affinity = colon + 1;
            // 跳过空格
            while (*affinity == ' ' || *affinity == '\t') affinity++;
            
            // 移除换行符
            char *newline = strchr(affinity, '\n');
            if (newline) *newline = '\0';
            
            char *result = strdup(affinity);
            free(output);
            return result;
        }
        free(output);
    }
    
    return strdup("0-7");
}

int system_get_status(system_status_t *status) {
    if (!status) return -1;
    
    // 获取设备型号
    char *model = exec_command("getprop ro.product.model");
    if (model) {
        strncpy(status->device_model, model, sizeof(status->device_model) - 1);
        char *newline = strchr(status->device_model, '\n');
        if (newline) *newline = '\0';
        free(model);
    }
    
    // 获取SOC型号
    char *soc = exec_command("getprop ro.soc.model");
    if (soc) {
        strncpy(status->soc_model, soc, sizeof(status->soc_model) - 1);
        char *newline = strchr(status->soc_model, '\n');
        if (newline) *newline = '\0';
        free(soc);
    }
    
    // 获取CPU信息
    status->core_count = system_get_cpu_info(status->cores, 8);
    
    // 检查AppOpt状态
    status->appopt_running = system_is_appopt_running();
    status->appopt_pid = app_get_pid("AppOpt");
    
    return 0;
}

int system_get_cpu_info(cpu_core_t *cores, int max_count) {
    int count = 0;
    
    for (int i = 0; i < max_count; i++) {
        char path[256];
        
        // 检查CPU是否在线
        snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/online", i);
        FILE *fp = fopen(path, "r");
        if (!fp) break;
        
        int online;
        fscanf(fp, "%d", &online);
        fclose(fp);
        
        cores[count].id = i;
        cores[count].online = (online == 1);
        
        // 获取频率范围
        snprintf(path, sizeof(path), 
                "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_min_freq", i);
        fp = fopen(path, "r");
        if (fp) {
            fscanf(fp, "%d", &cores[count].min_freq);
            fclose(fp);
        }
        
        snprintf(path, sizeof(path), 
                "/sys/devices/system/cpu/cpu%d/cpufreq/cpuinfo_max_freq", i);
        fp = fopen(path, "r");
        if (fp) {
            fscanf(fp, "%d", &cores[count].max_freq);
            fclose(fp);
        }
        
        // 根据频率判断核心类型 (骁龙8Gen2)
        if (cores[count].max_freq < 2100000) {
            cores[count].type = CPU_TYPE_SMALL;  // 0-2
        } else if (cores[count].max_freq < 2900000) {
            cores[count].type = CPU_TYPE_MID;    // 3-4
        } else if (cores[count].max_freq < 3100000) {
            cores[count].type = CPU_TYPE_BIG;    // 5-6
        } else {
            cores[count].type = CPU_TYPE_PRIME;  // 7
        }
        
        count++;
    }
    
    return count;
}

bool system_is_appopt_running(void) {
    return app_is_running("AppOpt");
}
