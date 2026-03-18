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
    LOG_INFO("API: /api/apps 被调用");
    
    // 直接返回静态JSON数据用于测试
    const char *json_template = 
        "{"
        "\"success\":true,"
        "\"total\":2,"
        "\"apps\":["
        "{"
        "\"package\":\"com.tencent.mm\","
        "\"name\":\"微信\","
        "\"affinity\":\"0-2\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.ss.android.ugc.aweme\","
        "\"name\":\"抖音\","
        "\"affinity\":\"0-3\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "}"
        "]"
        "}";
    
    LOG_INFO("返回静态数据");
    return strdup(json_template);
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
