#include "http_server.h"
#include "api.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Web文件路径
#define WEB_ROOT "/data/adb/modules/AppOpt_8Gen2_Mi13/web"

// 读取文件内容
static char* read_file(const char *filepath, size_t *size) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        LOG_ERROR("无法打开文件: %s", filepath);
        return NULL;
    }
    
    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return NULL;
    }
    
    *size = st.st_size;
    char *content = (char*)malloc(*size + 1);
    if (!content) {
        close(fd);
        return NULL;
    }
    
    if (read(fd, content, *size) != (ssize_t)*size) {
        free(content);
        close(fd);
        return NULL;
    }
    
    content[*size] = '\0';
    close(fd);
    return content;
}

// HTTP请求处理回调
static enum MHD_Result request_handler(void *cls,
                          struct MHD_Connection *connection,
                          const char *url,
                          const char *method,
                          const char *version,
                          const char *upload_data,
                          size_t *upload_data_size,
                          void **con_cls) {
    
    LOG_DEBUG("请求: %s %s", method, url);
    
    // 处理API请求
    if (strncmp(url, "/api/", 5) == 0) {
        return api_handle_request(connection, url, method, upload_data, upload_data_size);
    }
    
    // 处理静态文件
    char filepath[256];
    const char *content_type = "text/html";
    
    if (strcmp(url, "/") == 0 || strcmp(url, "/index.html") == 0) {
        snprintf(filepath, sizeof(filepath), "%s/index.html", WEB_ROOT);
        content_type = "text/html";
    }
    else if (strcmp(url, "/style.css") == 0) {
        snprintf(filepath, sizeof(filepath), "%s/style.css", WEB_ROOT);
        content_type = "text/css";
    }
    else if (strcmp(url, "/app.js") == 0) {
        snprintf(filepath, sizeof(filepath), "%s/app.js", WEB_ROOT);
        content_type = "application/javascript";
    }
    else {
        // 404
        const char *not_found = "{\"error\":\"Not Found\"}";
        return send_json_response(connection, not_found, 404);
    }
    
    // 读取文件
    size_t size;
    char *content = read_file(filepath, &size);
    if (!content) {
        const char *error = "{\"error\":\"File not found\"}";
        return send_json_response(connection, error, 404);
    }
    
    // 发送响应
    struct MHD_Response *response = MHD_create_response_from_buffer(
        size, content, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", content_type);
    int ret = MHD_queue_response(connection, 200, response);
    MHD_destroy_response(response);
    
    return ret;
}

http_server_t* http_server_init(int port) {
    http_server_t *server = (http_server_t*)malloc(sizeof(http_server_t));
    if (!server) {
        LOG_ERROR("分配内存失败");
        return NULL;
    }
    
    server->port = port;
    server->running = false;
    server->daemon = NULL;
    
    return server;
}

bool http_server_start(http_server_t *server) {
    if (!server) return false;
    
    server->daemon = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY,
        server->port,
        NULL, NULL,
        &request_handler, NULL,
        MHD_OPTION_END
    );
    
    if (!server->daemon) {
        LOG_ERROR("启动MHD守护进程失败");
        return false;
    }
    
    server->running = true;
    return true;
}

void http_server_stop(http_server_t *server) {
    if (!server) return;
    
    server->running = false;
    if (server->daemon) {
        MHD_stop_daemon(server->daemon);
        server->daemon = NULL;
    }
}

void http_server_free(http_server_t *server) {
    if (!server) return;
    
    http_server_stop(server);
    free(server);
}

enum MHD_Result send_json_response(struct MHD_Connection *connection, 
                       const char *json_data, 
                       int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(json_data),
        (void*)json_data,
        MHD_RESPMEM_MUST_COPY
    );
    
    MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    
    return ret;
}

enum MHD_Result send_html_response(struct MHD_Connection *connection, 
                       const char *html_data, 
                       int status_code) {
    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(html_data),
        (void*)html_data,
        MHD_RESPMEM_MUST_COPY
    );
    
    MHD_add_response_header(response, "Content-Type", "text/html; charset=utf-8");
    
    int ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);
    
    return ret;
}
