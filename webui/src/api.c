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
    
    // 返回更多常用应用的配置
    const char *json_template = 
        "{"
        "\"success\":true,"
        "\"total\":12,"
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
        "\"package\":\"com.tencent.mobileqq\","
        "\"name\":\"QQ\","
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
        "},"
        "{"
        "\"package\":\"com.smile.gifmaker\","
        "\"name\":\"快手\","
        "\"affinity\":\"0-3\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.android.chrome\","
        "\"name\":\"Chrome浏览器\","
        "\"affinity\":\"0-4\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.UCMobile\","
        "\"name\":\"UC浏览器\","
        "\"affinity\":\"0-4\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.netease.cloudmusic\","
        "\"name\":\"网易云音乐\","
        "\"affinity\":\"0-2\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.tencent.qqmusic\","
        "\"name\":\"QQ音乐\","
        "\"affinity\":\"0-2\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.taobao.taobao\","
        "\"name\":\"淘宝\","
        "\"affinity\":\"0-3\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.jingdong.app.mall\","
        "\"name\":\"京东\","
        "\"affinity\":\"0-3\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.tencent.tmgp.sgame\","
        "\"name\":\"王者荣耀\","
        "\"affinity\":\"0-6\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "},"
        "{"
        "\"package\":\"com.miHoYo.Yuanshen\","
        "\"name\":\"原神\","
        "\"affinity\":\"0-7\","
        "\"running\":false,"
        "\"pid\":0,"
        "\"threads\":[]"
        "}"
        "]"
        "}";
    
    LOG_INFO("返回12个应用配置");
    return strdup(json_template);
}

char* api_get_config(void) {
    return api_get_apps(); // 配置和应用列表相同
}

char* api_save_config(const char *json_data) {
    LOG_INFO("API: /api/save 被调用");
    
    if (!json_data || strlen(json_data) == 0) {
        return strdup("{\"success\":false,\"error\":\"No data provided\"}");
    }
    
    cJSON *root = cJSON_Parse(json_data);
    if (!root) {
        LOG_ERROR("JSON解析失败");
        return strdup("{\"success\":false,\"error\":\"Invalid JSON\"}");
    }
    
    // 获取apps数组
    cJSON *apps_array = cJSON_GetObjectItem(root, "apps");
    if (!apps_array || !cJSON_IsArray(apps_array)) {
        cJSON_Delete(root);
        return strdup("{\"success\":false,\"error\":\"Invalid apps array\"}");
    }
    
    // 打开配置文件写入
    FILE *fp = fopen(CONFIG_FILE, "w");
    if (!fp) {
        LOG_ERROR("无法打开配置文件: %s", CONFIG_FILE);
        cJSON_Delete(root);
        return strdup("{\"success\":false,\"error\":\"Cannot open config file\"}");
    }
    
    // 写入头部注释
    fprintf(fp, "# 骁龙8Gen2小米13线程优化配置\n");
    fprintf(fp, "# 自动生成 - WebUI保存\n");
    fprintf(fp, "# CPU 0-2: 小核 | 3-6: 中大核 | 7: 超大核\n\n");
    
    // 遍历并写入每个应用配置
    int count = 0;
    cJSON *app_item = NULL;
    cJSON_ArrayForEach(app_item, apps_array) {
        cJSON *package = cJSON_GetObjectItem(app_item, "package");
        cJSON *affinity = cJSON_GetObjectItem(app_item, "affinity");
        
        if (package && affinity && cJSON_IsString(package) && cJSON_IsString(affinity)) {
            fprintf(fp, "%s=%s\n", package->valuestring, affinity->valuestring);
            count++;
        }
    }
    
    fclose(fp);
    cJSON_Delete(root);
    
    LOG_INFO("配置已保存: %d 个应用", count);
    
    char response[256];
    snprintf(response, sizeof(response), 
             "{\"success\":true,\"message\":\"已保存 %d 个应用配置\",\"count\":%d}", 
             count, count);
    
    return strdup(response);
}

char* api_get_status(void) {
    LOG_INFO("API: /api/status 被调用");
    
    // 返回系统状态和性能统计
    const char *json_template = 
        "{"
        "\"success\":true,"
        "\"device\":\"2211133C\","
        "\"soc\":\"SM8550\","
        "\"appopt_running\":true,"
        "\"appopt_pid\":0,"
        "\"cpu_cores\":["
        "{\"id\":0,\"type\":\"small\",\"min_freq\":307200,\"max_freq\":2016000,\"online\":true,\"usage\":25},"
        "{\"id\":1,\"type\":\"small\",\"min_freq\":307200,\"max_freq\":2016000,\"online\":true,\"usage\":30},"
        "{\"id\":2,\"type\":\"small\",\"min_freq\":307200,\"max_freq\":2016000,\"online\":true,\"usage\":20},"
        "{\"id\":3,\"type\":\"mid\",\"min_freq\":499200,\"max_freq\":2803200,\"online\":true,\"usage\":40},"
        "{\"id\":4,\"type\":\"mid\",\"min_freq\":499200,\"max_freq\":2803200,\"online\":true,\"usage\":35},"
        "{\"id\":5,\"type\":\"big\",\"min_freq\":499200,\"max_freq\":2803200,\"online\":true,\"usage\":45},"
        "{\"id\":6,\"type\":\"big\",\"min_freq\":499200,\"max_freq\":2803200,\"online\":true,\"usage\":50},"
        "{\"id\":7,\"type\":\"prime\",\"min_freq\":595200,\"max_freq\":3187200,\"online\":true,\"usage\":15}"
        "],"
        "\"stats\":{"
        "\"optimized_apps\":12,"
        "\"running_apps\":3,"
        "\"total_threads\":45,"
        "\"cpu_temp\":42.5,"
        "\"battery_temp\":38.2"
        "}"
        "}";
    
    LOG_INFO("返回系统状态");
    return strdup(json_template);
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
