#include "api.h"
#include "app_manager.h"
#include "config.h"
#include "http_server.h"
#include <cJSON.h>

enum MHD_Result api_handle_request(struct MHD_Connection *connection,
                       const char *url,
                       const char *method,
                       const char *upload_data,
                       size_t *upload_data_size) {
    
    char *response_json = NULL;
    
    // GET /api/apps
    if (strcmp(url, "/api/apps") == 0 && strcmp(method, "GET") == 0) {
        response_json = api_get_apps();
    }
    // GET /api/config
    else if (strcmp(url, "/api/config") == 0 && strcmp(method, "GET") == 0) {
        response_json = api_get_config();
    }
    // POST /api/save
    else if (strcmp(url, "/api/save") == 0 && strcmp(method, "POST") == 0) {
        response_json = api_save_config(upload_data);
    }
    // GET /api/status
    else if (strcmp(url, "/api/status") == 0 && strcmp(method, "GET") == 0) {
        response_json = api_get_status();
    }
    // GET /api/presets
    else if (strcmp(url, "/api/presets") == 0 && strcmp(method, "GET") == 0) {
        response_json = api_get_presets();
    }
    // POST /api/apply_preset
    else if (strcmp(url, "/api/apply_preset") == 0 && strcmp(method, "POST") == 0) {
        response_json = api_apply_preset(upload_data);
    }
    else {
        response_json = strdup("{\"error\":\"Unknown API endpoint\"}");
    }
    
    int ret = send_json_response(connection, response_json, 200);
    free(response_json);
    
    return ret;
}

char* api_get_apps(void) {
    app_config_t apps[MAX_APPS];
    int app_count = 0;
    
    LOG_INFO("开始读取应用配置...");
    
    // 读取配置
    if (config_read(CONFIG_FILE, apps, &app_count) < 0) {
        LOG_ERROR("读取配置文件失败: %s", CONFIG_FILE);
        // 返回空列表而不是错误
        app_count = 0;
    }
    
    LOG_INFO("读取到 %d 个应用配置", app_count);
    
    // 如果没有配置，添加一些示例数据
    if (app_count == 0) {
        LOG_INFO("配置为空，添加示例数据");
        strcpy(apps[0].package, "com.tencent.mm");
        strcpy(apps[0].name, "微信");
        strcpy(apps[0].affinity, "0-2");
        apps[0].running = false;
        apps[0].pid = 0;
        apps[0].thread_count = 0;
        
        strcpy(apps[1].package, "com.ss.android.ugc.aweme");
        strcpy(apps[1].name, "抖音");
        strcpy(apps[1].affinity, "0-3");
        apps[1].running = false;
        apps[1].pid = 0;
        apps[1].thread_count = 0;
        
        app_count = 2;
    }
    
    // 更新运行状态
    for (int i = 0; i < app_count; i++) {
        apps[i].running = app_is_running(apps[i].package);
        if (apps[i].running) {
            apps[i].pid = app_get_pid(apps[i].package);
        }
    }
    
    // 构建JSON
    cJSON *root = cJSON_CreateObject();
    cJSON *apps_array = cJSON_CreateArray();
    
    for (int i = 0; i < app_count; i++) {
        cJSON *app_obj = cJSON_CreateObject();
        cJSON_AddStringToObject(app_obj, "package", apps[i].package);
        cJSON_AddStringToObject(app_obj, "name", apps[i].name);
        cJSON_AddStringToObject(app_obj, "affinity", apps[i].affinity);
        cJSON_AddBoolToObject(app_obj, "running", apps[i].running);
        cJSON_AddNumberToObject(app_obj, "pid", apps[i].pid);
        
        // 添加线程配置
        cJSON *threads_array = cJSON_CreateArray();
        for (int j = 0; j < apps[i].thread_count; j++) {
            cJSON *thread_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(thread_obj, "name", apps[i].threads[j].name);
            cJSON_AddStringToObject(thread_obj, "affinity", apps[i].threads[j].affinity);
            cJSON_AddItemToArray(threads_array, thread_obj);
        }
        cJSON_AddItemToObject(app_obj, "threads", threads_array);
        
        cJSON_AddItemToArray(apps_array, app_obj);
    }
    
    cJSON_AddItemToObject(root, "apps", apps_array);
    cJSON_AddNumberToObject(root, "total", app_count);
    cJSON_AddBoolToObject(root, "success", true);
    
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    
    LOG_INFO("返回JSON: %d 字节", (int)strlen(json_str));
    
    return json_str;
}

char* api_get_config(void) {
    return api_get_apps(); // 配置和应用列表相同
}

char* api_save_config(const char *json_data) {
    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        return strdup("{\"success\":false,\"error\":\"Invalid JSON\"}");
    }
    
    // 解析JSON并保存配置
    // TODO: 实现配置保存逻辑
    
    cJSON_Delete(root);
    return strdup("{\"success\":true}");
}

char* api_get_status(void) {
    system_status_t status;
    system_get_status(&status);
    
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "device", status.device_model);
    cJSON_AddStringToObject(root, "soc", status.soc_model);
    cJSON_AddBoolToObject(root, "appopt_running", status.appopt_running);
    cJSON_AddNumberToObject(root, "appopt_pid", status.appopt_pid);
    
    // CPU核心信息
    cJSON *cores_array = cJSON_CreateArray();
    for (int i = 0; i < status.core_count; i++) {
        cJSON *core_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(core_obj, "id", status.cores[i].id);
        
        const char *type_str = "unknown";
        switch (status.cores[i].type) {
            case CPU_TYPE_SMALL: type_str = "small"; break;
            case CPU_TYPE_MID: type_str = "mid"; break;
            case CPU_TYPE_BIG: type_str = "big"; break;
            case CPU_TYPE_PRIME: type_str = "prime"; break;
        }
        cJSON_AddStringToObject(core_obj, "type", type_str);
        cJSON_AddNumberToObject(core_obj, "min_freq", status.cores[i].min_freq);
        cJSON_AddNumberToObject(core_obj, "max_freq", status.cores[i].max_freq);
        cJSON_AddBoolToObject(core_obj, "online", status.cores[i].online);
        
        cJSON_AddItemToArray(cores_array, core_obj);
    }
    cJSON_AddItemToObject(root, "cpu_cores", cores_array);
    
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    
    return json_str;
}

char* api_get_presets(void) {
    cJSON *root = cJSON_CreateObject();
    cJSON *presets_array = cJSON_CreateArray();
    
    // 游戏模式
    cJSON *gaming = cJSON_CreateObject();
    cJSON_AddStringToObject(gaming, "id", "gaming");
    cJSON_AddStringToObject(gaming, "name", "游戏模式");
    cJSON_AddStringToObject(gaming, "description", "主进程小核，渲染大核，极致性能");
    cJSON_AddItemToArray(presets_array, gaming);
    
    // 日常模式
    cJSON *daily = cJSON_CreateObject();
    cJSON_AddStringToObject(daily, "id", "daily");
    cJSON_AddStringToObject(daily, "name", "日常模式");
    cJSON_AddStringToObject(daily, "description", "均衡性能与功耗");
    cJSON_AddItemToArray(presets_array, daily);
    
    // 省电模式
    cJSON *powersave = cJSON_CreateObject();
    cJSON_AddStringToObject(powersave, "id", "powersave");
    cJSON_AddStringToObject(powersave, "name", "省电模式");
    cJSON_AddStringToObject(powersave, "description", "优先使用小核，降低功耗");
    cJSON_AddItemToArray(presets_array, powersave);
    
    cJSON_AddItemToObject(root, "presets", presets_array);
    
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    
    return json_str;
}

char* api_apply_preset(const char *json_data) {
    // TODO: 实现预设方案应用
    return strdup("{\"success\":true}");
}
