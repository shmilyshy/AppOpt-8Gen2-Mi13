@echo off
REM 上传项目到GitHub的辅助脚本
REM 作者: 刘十三

echo =========================================
echo 上传 AppOpt 项目到 GitHub
echo =========================================
echo.

REM 检查Git是否安装
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo 错误: 未找到Git
    echo 请先安装Git: https://git-scm.com/download/win
    echo.
    echo 或者使用网页上传方式，查看: GitHub自动编译指南.md
    pause
    exit /b 1
)

echo ✓ Git已安装
git --version
echo.

REM 检查是否已初始化
if not exist .git (
    echo 初始化Git仓库...
    git init
    echo.
    
    echo 请输入你的GitHub仓库地址:
    echo 格式: https://github.com/你的用户名/AppOpt-8Gen2-Mi13.git
    set /p REPO_URL="仓库地址: "
    
    git remote add origin %REPO_URL%
    echo ✓ 已添加远程仓库
    echo.
)

REM 添加文件
echo 添加文件到Git...
git add .github/
git add webui/
git add README.md
git add .gitignore

REM 添加中文文件名（使用通配符避免编码问题）
git add *.md
git add *.bat

echo ✓ 文件已添加
echo.

REM 提交
echo 提交更改...
set /p COMMIT_MSG="提交信息 (直接回车使用默认): "
if "%COMMIT_MSG%"=="" set COMMIT_MSG=Update: AppOpt WebUI source code

git commit -m "%COMMIT_MSG%"
echo ✓ 已提交
echo.

REM 推送
echo 推送到GitHub...
echo.
echo 注意: 如果提示需要登录
echo   用户名: 你的GitHub用户名
echo   密码: 使用Personal Access Token (不是密码)
echo.
echo 如何获取Token:
echo   1. GitHub → Settings → Developer settings
echo   2. Personal access tokens → Generate new token
echo   3. 勾选 repo 权限
echo   4. 复制token作为密码使用
echo.
pause

git push -u origin main

if %ERRORLEVEL% EQU 0 (
    echo.
    echo =========================================
    echo ✓ 上传成功!
    echo =========================================
    echo.
    echo 下一步:
    echo 1. 访问你的GitHub仓库
    echo 2. 点击 Actions 标签
    echo 3. 查看编译进度
    echo 4. 编译完成后下载 AppOptWebUI
    echo.
    echo 详细说明请查看: GitHub自动编译指南.md
    echo.
) else (
    echo.
    echo =========================================
    echo ✗ 上传失败
    echo =========================================
    echo.
    echo 可能的原因:
    echo 1. 仓库地址错误
    echo 2. 没有权限
    echo 3. 网络问题
    echo 4. 需要Personal Access Token
    echo.
    echo 请查看错误信息，或使用网页上传方式
    echo 详细说明: GitHub自动编译指南.md
    echo.
)

pause
