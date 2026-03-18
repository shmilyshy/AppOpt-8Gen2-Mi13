# GitHub Actions 快速开始

## 🎯 最简单的方法 (推荐)

### 你只需要做3件事：

#### 1️⃣ 注册GitHub账号
- 访问: https://github.com
- 点击 "Sign up" 注册
- 验证邮箱

#### 2️⃣ 创建仓库并上传文件
- 登录GitHub
- 点击 "+" → "New repository"
- 仓库名: `AppOpt-8Gen2-Mi13`
- 选择 Private (私有)
- 点击 "Create repository"
- 点击 "uploading an existing file"
- 拖拽这些文件夹到页面：
  ```
  .github/
  webui/
  ```
- 点击 "Commit changes"

#### 3️⃣ 等待编译完成
- 点击 "Actions" 标签
- 等待编译完成 (约10分钟)
- 下载 "AppOptWebUI-arm64"

## 📦 需要上传的文件

### 必须上传
```
.github/workflows/build-webui.yml  ✅ 已创建
webui/src/*.c                      ✅ 已创建 (5个文件)
webui/include/*.h                  ✅ 已创建 (5个文件)
webui/web/*                        ✅ 已创建 (3个文件)
webui/Makefile                     ✅ 已创建
webui/build.sh                     ✅ 已创建
```

### 可选上传
```
README.md                          ✅ 项目说明
.gitignore                         ✅ 忽略文件
GitHub自动编译指南.md              ✅ 详细指南
```

## 🖱️ 网页上传步骤 (最简单)

### 第1步: 创建仓库
1. 登录 https://github.com
2. 点击右上角 "+" → "New repository"
3. 填写：
   - Repository name: `AppOpt-8Gen2-Mi13`
   - Description: `骁龙8Gen2小米13线程优化`
   - 选择 **Private** (私有仓库)
4. 点击 "Create repository"

### 第2步: 上传文件
1. 在新建的仓库页面
2. 点击 "uploading an existing file"
3. 打开文件管理器，找到项目文件夹
4. 选中 `.github` 和 `webui` 两个文件夹
5. 拖拽到浏览器页面
6. 等待上传完成
7. 在底部填写: "Initial commit"
8. 点击 "Commit changes"

### 第3步: 查看编译
1. 点击顶部 "Actions" 标签
2. 看到 "Build AppOpt WebUI" 正在运行
3. 点击进入查看详细进度
4. 等待约10分钟

### 第4步: 下载结果
1. 编译完成后（绿色✅）
2. 滚动到页面底部
3. 找到 "Artifacts" 部分
4. 点击 "AppOptWebUI-arm64" 下载
5. 解压得到 `AppOptWebUI` 文件

## 💻 命令行上传 (可选)

如果你熟悉命令行，可以使用脚本：

### Windows
```cmd
# 双击运行
上传到GitHub.bat
```

### 手动命令
```bash
# 1. 初始化
git init
git remote add origin https://github.com/你的用户名/AppOpt-8Gen2-Mi13.git

# 2. 添加文件
git add .github/ webui/ README.md .gitignore

# 3. 提交
git commit -m "Initial commit"

# 4. 推送
git push -u origin main
```

## 🎬 视频教程 (文字版)

### 场景1: 第一次使用GitHub
```
1. 打开浏览器 → https://github.com
2. 点击 "Sign up" → 填写邮箱、密码、用户名
3. 验证邮箱 → 登录
4. 点击 "+" → "New repository"
5. 填写仓库名 → 选择Private → Create
6. 点击 "uploading an existing file"
7. 拖拽 .github 和 webui 文件夹
8. 点击 "Commit changes"
9. 点击 "Actions" → 等待编译
10. 下载 Artifacts
```

### 场景2: 已有GitHub账号
```
1. 登录GitHub
2. 新建仓库 → 上传文件
3. 等待编译 → 下载结果
```

## ⏱️ 时间估算

- 注册GitHub: 2分钟
- 创建仓库: 1分钟
- 上传文件: 2分钟
- 等待编译: 10分钟
- **总计: 约15分钟**

## ✅ 成功标志

### 上传成功
- 仓库中能看到 `.github` 和 `webui` 文件夹
- 文件结构完整

### 编译成功
- Actions页面显示绿色✅
- 有 "Artifacts" 可下载
- 下载的文件约5-8MB

### 运行成功
```bash
# 推送到设备
adb push AppOptWebUI /data/adb/modules/AppOpt_8Gen2_Mi13/
adb shell su -c "chmod 755 /data/adb/modules/AppOpt_8Gen2_Mi13/AppOptWebUI"

# 启动
adb shell su -c "/data/adb/modules/AppOpt_8Gen2_Mi13/AppOptWebUI -d"

# 访问
浏览器打开: http://手机IP:8080
```

## 🐛 常见问题

### Q: 上传时文件夹是空的
**A**: 确保选中整个文件夹，不是文件夹里的文件

### Q: 编译失败
**A**: 
1. 检查文件结构是否完整
2. 查看Actions日志找错误
3. 确认所有必需文件都已上传

### Q: 找不到Artifacts
**A**: 
1. 确保编译成功（绿色✅）
2. 滚动到页面最底部
3. 只有成功的运行才有Artifacts

### Q: 下载的文件无法运行
**A**: 
1. 确保是ARM64版本
2. 检查文件权限 (chmod 755)
3. 确保在Android设备上运行

## 📱 完整流程示例

```
1. 注册GitHub (2分钟)
   ↓
2. 创建仓库 (1分钟)
   ↓
3. 上传文件 (2分钟)
   .github/ ✅
   webui/ ✅
   ↓
4. 自动编译 (10分钟)
   安装NDK ✅
   编译依赖 ✅
   编译WebUI ✅
   ↓
5. 下载结果 (1分钟)
   AppOptWebUI ✅
   ↓
6. 部署测试 (2分钟)
   推送到设备 ✅
   启动服务 ✅
   浏览器访问 ✅
```

## 🎉 优势

### 为什么用GitHub Actions？
- ✅ 完全免费
- ✅ 无需本地环境
- ✅ 自动编译
- ✅ 随时下载
- ✅ 版本管理
- ✅ 协作开发

### 对比其他方案
| 方案 | 难度 | 时间 | 成本 |
|------|------|------|------|
| GitHub Actions | ⭐ | 15分钟 | 免费 |
| WSL2编译 | ⭐⭐⭐⭐ | 1小时+ | 免费 |
| Docker编译 | ⭐⭐⭐ | 30分钟+ | 免费 |
| 本地Linux | ⭐⭐⭐⭐⭐ | 2小时+ | 免费 |

## 📞 需要帮助？

### 详细文档
- `GitHub自动编译指南.md` - 完整详细指南
- `最终交付说明.md` - 项目总览

### 在线资源
- GitHub文档: https://docs.github.com
- GitHub Actions: https://docs.github.com/actions

### 问题排查
1. 查看Actions日志
2. 检查文件结构
3. 确认配置正确

---

**作者**: 刘十三  
**日期**: 2026-03-18  
**预计时间**: 15分钟  
**难度**: ⭐ 简单
