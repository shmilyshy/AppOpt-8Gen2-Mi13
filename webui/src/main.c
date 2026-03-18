#include "common.h"
#include "http_server.h"
#include "api.h"
#include <signal.h>

static http_server_t *g_server = NULL;

// 信号处理
void signal_handler(int signum) {
    LOG_INFO("收到信号 %d, 正在关闭服务器...", signum);
    if (g_server) {
        http_server_stop(g_server);
    }
    exit(0);
}

// 打印使用说明
void print_usage(const char *prog_name) {
    printf("AppOpt WebUI v%s - 骁龙8Gen2小米13线程优化管理界面\n", WEBUI_VERSION);
    printf("作者: 刘十三\n\n");
    printf("使用方法:\n");
    printf("  %s [选项]\n\n", prog_name);
    printf("选项:\n");
    printf("  -p <端口>    指定HTTP端口 (默认: %d)\n", WEBUI_PORT);
    printf("  -d           后台运行模式\n");
    printf("  -h           显示此帮助信息\n");
    printf("  -v           显示版本信息\n\n");
    printf("示例:\n");
    printf("  %s              # 使用默认端口8080\n", prog_name);
    printf("  %s -p 9090      # 使用端口9090\n", prog_name);
    printf("  %s -d           # 后台运行\n\n", prog_name);
    printf("访问地址:\n");
    printf("  http://localhost:%d\n", WEBUI_PORT);
    printf("  http://设备IP:%d\n", WEBUI_PORT);
}

int main(int argc, char *argv[]) {
    int port = WEBUI_PORT;
    bool daemon_mode = false;
    
    // 解析命令行参数
    int opt;
    while ((opt = getopt(argc, argv, "p:dhv")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                if (port <= 0 || port > 65535) {
                    fprintf(stderr, "错误: 无效的端口号 %s\n", optarg);
                    return 1;
                }
                break;
            case 'd':
                daemon_mode = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'v':
                printf("AppOpt WebUI v%s\n", WEBUI_VERSION);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // 后台运行
    if (daemon_mode) {
        pid_t pid = fork();
        if (pid < 0) {
            LOG_ERROR("fork失败");
            return 1;
        }
        if (pid > 0) {
            printf("后台进程已启动, PID: %d\n", pid);
            return 0;
        }
        setsid();
        chdir("/");
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    
    // 注册信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    LOG_INFO("AppOpt WebUI v%s 启动中...", WEBUI_VERSION);
    LOG_INFO("作者: 刘十三");
    
    // 初始化HTTP服务器
    g_server = http_server_init(port);
    if (!g_server) {
        LOG_ERROR("初始化HTTP服务器失败");
        return 1;
    }
    
    // 启动服务器
    if (!http_server_start(g_server)) {
        LOG_ERROR("启动HTTP服务器失败");
        http_server_free(g_server);
        return 1;
    }
    
    LOG_INFO("服务器已启动在端口 %d", port);
    LOG_INFO("访问地址: http://localhost:%d", port);
    LOG_INFO("按 Ctrl+C 停止服务器");
    
    // 保持运行
    while (g_server->running) {
        sleep(1);
    }
    
    // 清理资源
    http_server_free(g_server);
    LOG_INFO("服务器已关闭");
    
    return 0;
}
