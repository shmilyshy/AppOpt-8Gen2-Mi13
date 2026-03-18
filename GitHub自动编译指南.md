# GitHub Actions 自动编译 WebUI 指南

## 📋 准备工作

### 1. 注册GitHub账号
如果还没有GitHub账号：
1. 访问: https://github.com
2. 点击 "Sign up" 注册
3. 填写邮箱、密码、用户名
4. 验证邮箱

### 2. 安装Git (可选)
如果想用命令行上传：
- Windows: https://git-scm.com/download/win
- 或者直接用GitHub网页上传

## 🎯 方法1: 网页上传 (最简单)

### 步骤1: 创建仓库
1. 登录GitHub
2. 点击右上角 "+" → "New repository"
3. 填写信息：
   - Repository name: `AppOpt-8Gen2-Mi13`
   - Description: `骁龙8Gen2小米13线程优化模块`
   - 选择 Private (私有) 或 Public (公开)
4. 点击 "Create repository"

### 步骤2: 上传文件
1. 在仓库页面，点击 "uploading an existing file"
2. 拖拽以下文件夹到页面：
   ```
   .github/
   webui/
   README.md
   ```
3. 或者点击 "choose your files" 选择文件
4. 填写提交信息: "Initial commit"
5. 点击 "Commit changes"

### 步骤3: 触发编译
上传完成后，GitHub Actions会自动开始编译！

查看编译进度：
1. 点击仓库顶部的 "Actions" 标签
2. 看到 "Build AppOpt WebUI" 工作流
3. 点击查看详细日志

### 步骤4: 下载编译结果
编译完成后（约5-10分钟）：
1. 在 Actions 页面找到成功的运行
2. 滚动到底部 "Artifacts" 部分
3. 下载 "AppOptWebUI-arm64"
4. 解压得到 `AppOptWebUI` 二进制文件

## 🎯 方法2: Git命令行上传

### 步骤1: 初始化仓库
```bash
# 进入项目目录
cd C:\Users\你的用户名\Desktop\shy\6+2线程v8

# 初始化Git
git init

# 添加远程仓库 (替换为你的仓库地址)
git remote add origin https://github.com/你的用户名/AppOpt-8Gen2-Mi13.git
```

### 步骤2: 准备上传文件
```bash
# 创建 .gitignore 文件
echo "*.zip" > .gitignore
echo "*.log" >> .gitignore
echo "bin/" >> .gitignore
echo "build/" >> .gitignore

# 添加文件
git add .github/
git add webui/
git add README.md
git add .gitignore
```

### 步骤3: 提交并推送
```bash
# 提交
git commit -m "Initial commit: AppOpt WebUI"

# 推送到GitHub
git push -u origin main
```

如果提示需要登录：
- 用户名: 你的GitHub用户名
- 密码: 使用Personal Access Token (不是密码)

### 创建Personal Access Token
1. GitHub → Settings → Developer settings
2. Personal access tokens → Tokens (classic)
3. Generate new token
4. 勾选 `repo` 权限
5. 生成并复制token
6. 用这个token作为密码

## 📁 需要上传的文件结构

```
你的仓库/
├── .github/
│   └── workflows/
│       └── build-webui.yml          # ✅ 已创建
│
├── webui/
│   ├── src/
│   │   ├── main.c                   # ✅ 已创建
│   │   ├── http_server.c            # ✅ 已创建
│   │   ├── api.c                    # ✅ 已创建
│   │   ├── config.c                 # ✅ 已创建
│   │   └── app_manager.c            # ✅ 已创建
│   │
│   ├── include/
│   │   ├── common.h                 # ✅ 已创建
│   │   ├── http_server.h            # ✅ 已创建
│   │   ├── api.h                    # ✅ 已创建
│   │   ├── config.h                 # ✅ 已创建
│   │   └── app_manager.h            # ✅ 已创建
│   │
│   ├── web/
│   │   ├── index.html               # ✅ 已创建
│   │   ├── style.css                # ✅ 已创建
│   │   └── app.js                   # ✅ 已创建
│   │
│   ├── Makefile                     # ✅ 已创建
│   ├── build.sh                     # ✅ 已创建
│   └── README.md                    # ✅ 已创建
│
├── README.md                        # ✅ 已创建
└── .gitignore                       # 需要创建
```

## 🔧 GitHub Actions 工作流程

### 自动触发条件
编译会在以下情况自动触发：
1. 推送代码到 main/master 分支
2. webui/ 目录下的文件有变化
3. 手动触发 (Actions页面点击 "Run workflow")

### 编译过程
1. ✅ 设置Ubuntu环境
2. ✅ 安装Android NDK
3. ✅ 编译libmicrohttpd
4. ✅ 编译cJSON
5. ✅ 编译WebUI
6. ✅ 上传编译结果

### 编译时间
- 首次编译: 约10-15分钟 (需要编译依赖库)
- 后续编译: 约5-8分钟 (有缓存)

## 📥 下载编译结果

### 方法1: 从Actions下载
1. 进入仓库的 Actions 页面
2. 点击最新的成功运行
3. 滚动到底部 "Artifacts"
4. 下载 "AppOptWebUI-arm64"

### 方法2: 从Releases下载 (如果配置了)
1. 进入仓库的 Releases 页面
2. 下载最新版本的附件

## 🎨 自定义编译配置

### 修改编译选项
编辑 `.github/workflows/build-webui.yml`:

```yaml
# 修改NDK版本
- name: Setup Android NDK
  uses: nttld/setup-ndk@v1
  with:
    ndk-version: r25c  # 改为其他版本

# 修改编译参数
- name: Build WebUI
  run: |
    cd webui
    export NDK_PATH=$ANDROID_NDK_HOME
    make ARCH=arm64 CFLAGS="-O3" -j$(nproc)  # 添加优化选项
```

### 添加自动发布
在 `.github/workflows/build-webui.yml` 末尾添加：

```yaml
- name: Create Release
  if: github.event_name == 'push' && github.ref == 'refs/heads/main'
  uses: softprops/action-gh-release@v1
  with:
    tag_name: v${{ github.run_number }}
    files: webui/bin/AppOptWebUI
  env:
    GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

## 🐛 常见问题

### Q1: 编译失败
**A**: 查看Actions日志，找到错误信息
- 点击失败的运行
- 展开失败的步骤
- 查看详细错误

### Q2: 找不到Artifacts
**A**: 确保编译成功完成
- 绿色✅表示成功
- 红色❌表示失败
- 只有成功的运行才有Artifacts

### Q3: 编译时间太长
**A**: 正常现象
- 首次编译需要下载和编译依赖库
- 后续编译会快很多

### Q4: 如何手动触发编译
**A**: 
1. 进入Actions页面
2. 选择 "Build AppOpt WebUI"
3. 点击 "Run workflow"
4. 选择分支
5. 点击 "Run workflow" 按钮

### Q5: 私有仓库可以用吗
**A**: 可以！
- GitHub Actions对私有仓库也免费
- 每月有2000分钟的免费额度
- 编译一次约10分钟，足够用

## 📊 GitHub Actions 配额

### 免费额度
- Public仓库: 无限制
- Private仓库: 每月2000分钟
- 存储空间: 500MB

### 使用情况
- 每次编译: 约10分钟
- 每月可编译: 约200次
- 完全够用！

## 🎯 推荐工作流程

### 开发流程
1. 本地修改代码
2. 推送到GitHub
3. 自动编译
4. 下载二进制
5. 测试
6. 重复

### 发布流程
1. 确保代码稳定
2. 推送到main分支
3. 等待编译完成
4. 下载编译结果
5. 测试验证
6. 创建Release发布

## 📝 .gitignore 文件

创建 `.gitignore` 文件，避免上传不必要的文件：

```gitignore
# 编译产物
*.o
*.a
*.so
bin/
build/
AppOptWebUI

# 压缩包
*.zip
*.tar.gz
*.7z

# 日志
*.log

# 临时文件
*.tmp
*.bak
*~

# IDE
.vscode/
.idea/
*.swp

# 系统文件
.DS_Store
Thumbs.db
```

## 🎉 完成！

按照以上步骤，你就可以：
1. ✅ 自动编译WebUI
2. ✅ 无需本地环境
3. ✅ 随时下载最新版本
4. ✅ 完全免费

## 📞 需要帮助？

如果遇到问题：
1. 查看Actions日志
2. 检查文件结构
3. 确认配置正确
4. 查看GitHub文档: https://docs.github.com/actions

---

**作者**: 刘十三  
**日期**: 2026-03-18  
**版本**: v1.0
