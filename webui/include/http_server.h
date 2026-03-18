#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "common.h"
#include <microhttpd.h>

// HTTP响应类型
typedef enum {
    RESPONSE_JSON,
    RESPONSE_HTML,
    RESPONSE_CSS,
    RESPONSE_JS
} response_type_t;

// HTTP服务器结构
typedef struct {
    struct MHD_Daemon *daemon;
    int port;
    bool running;
} http_server_t;

// 初始化HTTP服务器
http_server_t* http_server_init(int port);

// 启动服务器
bool http_server_start(http_server_t *server);

// 停止服务器
void http_server_stop(http_server_t *server);

// 释放服务器资源
void http_server_free(http_server_t *server);

// 发送JSON响应
int send_json_response(struct MHD_Connection *connection, 
                       const char *json_data, 
                       int status_code);

// 发送HTML响应
int send_html_response(struct MHD_Connection *connection, 
                       const char *html_data, 
                       int status_code);

// 发送文件响应
int send_file_response(struct MHD_Connection *connection, 
                       const char *file_path, 
                       response_type_t type);

#endif // HTTP_SERVER_H
