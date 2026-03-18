#include "http_server.h"
#include "api.h"
#include <sys/stat.h>

// 嵌入的HTML内容
extern const char _binary_web_index_html_start[];
extern const char _binary_web_index_html_end[];
extern const char _binary_web_style_css_start[];
extern const char _binary_web_style_css_end[];
extern const char _binary_web_app_js_start[];
extern const char _binary_web_app_js_end[];

// HTTP请求处理回调
static int request_handler(void *cls,
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
    if (strcmp(url, "/") == 0 || strcmp(url, "/index.html") == 0) {
        size_t size = _binary_web_index_html_end - _binary_web_index_html_start;
        return send_html_response(connection, _binary_web_index_html_start, 200);
    }
    else if (strcmp(url, "/style.css") == 0) {
        size_t size = _binary_web_style_css_end - _binary_web_style_css_start;
        struct MHD_Response *response = MHD_create_response_from_buffer(
            size, (void*)_binary_web_style_css_start, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "text/css");
        int ret = MHD_queue_response(connection, 200, response);
        MHD_destroy_response(response);
        return ret;
    }
    else if (strcmp(url, "/app.js") == 0) {
        size_t size = _binary_web_app_js_end - _binary_web_app_js_start;
        struct MHD_Response *response = MHD_create_response_from_buffer(
            size, (void*)_binary_web_app_js_start, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "application/javascript");
        int ret = MHD_queue_response(connection, 200, response);
        MHD_destroy_response(response);
        return ret;
    }
    
    // 404
    const char *not_found = "{\"error\":\"Not Found\"}";
    return send_json_response(connection, not_found, 404);
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

int send_json_response(struct MHD_Connection *connection, 
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

int send_html_response(struct MHD_Connection *connection, 
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
