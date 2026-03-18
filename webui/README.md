# AppOpt WebUI - C语言实现

## 项目简介
基于C语言开发的轻量级WebUI管理界面，用于可视化管理骁龙8Gen2小米13的线程优化配置。

## 技术栈
- **后端**: C + libmicrohttpd (HTTP服务器)
- **前端**: HTML + CSS + JavaScript (嵌入式)
- **数据格式**: JSON (cJSON库)
- **目标平台**: Android ARM64
- **预期大小**: ~500KB单一二进制

## 功能特性
1. 📱 显示已安装应用列表（带图标）
2. 🔄 实时显示应用运行状态
3. ⚙️ 可视化线程绑定配置
4. 🎮 预设优化方案（游戏/日常/省电）
5. 💾 实时保存配置无需重启

## 项目结构
```
webui/
├── src/              # C源代码
│   ├── main.c        # 主程序入口
│   ├── http_server.c # HTTP服务器
│   ├── api.c         # API接口实现
│   ├── config.c      # 配置文件读写
│   └── app_manager.c # 应用管理
├── include/          # 头文件
│   ├── http_server.h
│   ├── api.h
│   ├── config.h
│   └── app_manager.h
├── web/              # 前端资源
│   ├── index.html    # 主页面
│   ├── style.css     # 样式
│   └── app.js        # 前端逻辑
├── Makefile          # 编译配置
└── README.md         # 本文件
```

## API接口设计

### 1. 获取应用列表
```
GET /api/apps
返回: {
  "apps": [
    {
      "package": "com.tencent.mm",
      "name": "微信",
      "icon": "base64...",
      "running": true,
      "pid": 12345,
      "cpu_affinity": "0-2"
    }
  ]
}
```

### 2. 获取配置
```
GET /api/config
返回: {
  "rules": [
    {
      "package": "com.tencent.mm",
      "affinity": "0-2",
      "threads": [
        {"name": "RenderThread", "affinity": "3-4"}
      ]
    }
  ]
}
```

### 3. 保存配置
```
POST /api/save
请求: {
  "package": "com.tencent.mm",
  "affinity": "0-2",
  "threads": [...]
}
返回: {"success": true}
```

### 4. 系统状态
```
GET /api/status
返回: {
  "device": "2211133C",
  "soc": "SM8550",
  "cpu_cores": [
    {"id": 0, "type": "small", "freq": "307-2016MHz"},
    {"id": 7, "type": "prime", "freq": "595-3187MHz"}
  ],
  "appopt_running": true
}
```

## 编译说明

### 依赖库
```bash
# Android NDK
# libmicrohttpd (HTTP服务器)
# cJSON (JSON解析)
```

### 交叉编译
```bash
# 使用Android NDK编译
export NDK_PATH=/path/to/ndk
make clean
make ARCH=arm64
```

### 输出
```
webui/AppOptWebUI  # 单一可执行文件 (~500KB)
```

## 使用方法

### 1. 启动WebUI服务
```bash
# 在Android设备上运行
su
cd /data/adb/modules/AppOpt_8Gen2_Mi13
./AppOptWebUI &
```

### 2. 访问界面
```
浏览器打开: http://localhost:8080
或通过局域网: http://设备IP:8080
```

### 3. 配置管理
- 在界面中选择应用
- 设置CPU核心绑定
- 点击保存即时生效

## 开发计划

### Phase 1: 基础框架 (1-2天)
- [x] 项目结构搭建
- [ ] HTTP服务器基础实现
- [ ] 静态文件服务
- [ ] 基础路由

### Phase 2: API开发 (2-3天)
- [ ] 应用列表获取
- [ ] 配置文件读写
- [ ] JSON数据处理
- [ ] 进程状态查询

### Phase 3: 前端开发 (2-3天)
- [ ] 界面设计
- [ ] 应用列表展示
- [ ] 配置编辑器
- [ ] 实时状态更新

### Phase 4: 优化测试 (1-2天)
- [ ] 性能优化
- [ ] 内存优化
- [ ] 错误处理
- [ ] 实机测试

## 作者
刘十三

## 许可
与AppOpt主模块相同
