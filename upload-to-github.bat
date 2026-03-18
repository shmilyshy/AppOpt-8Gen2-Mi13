@echo off
chcp 65001 >nul
REM Upload to GitHub - Simple Version
REM Author: Liu Shisan

echo =========================================
echo Upload AppOpt to GitHub
echo =========================================
echo.

REM Check Git
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: Git not found
    echo Please install Git: https://git-scm.com/download/win
    echo.
    echo Or use web upload method, see: GitHub快速开始.md
    pause
    exit /b 1
)

echo [OK] Git installed
git --version
echo.

REM Check if already initialized
if not exist .git (
    echo Initializing Git repository...
    git init
    echo.
    
    echo Please enter your GitHub repository URL:
    echo Format: https://github.com/username/AppOpt-8Gen2-Mi13.git
    set /p REPO_URL="Repository URL: "
    
    if "%REPO_URL%"=="" (
        echo Error: Repository URL cannot be empty
        pause
        exit /b 1
    )
    
    git remote add origin %REPO_URL%
    echo [OK] Remote repository added
    echo.
)

REM Add files
echo Adding files to Git...
git add .github/
git add webui/
git add README.md
git add .gitignore
git add *.md
git add *.bat

echo [OK] Files added
echo.

REM Commit
echo Committing changes...
set /p COMMIT_MSG="Commit message (press Enter for default): "
if "%COMMIT_MSG%"=="" set COMMIT_MSG=Update: AppOpt WebUI source code

git commit -m "%COMMIT_MSG%"
echo [OK] Committed
echo.

REM Push
echo Pushing to GitHub...
echo.
echo Note: If login required
echo   Username: Your GitHub username
echo   Password: Use Personal Access Token (not password)
echo.
echo How to get Token:
echo   1. GitHub -^> Settings -^> Developer settings
echo   2. Personal access tokens -^> Generate new token
echo   3. Check 'repo' permission
echo   4. Copy token and use as password
echo.
pause

git push -u origin main

if %ERRORLEVEL% EQU 0 (
    echo.
    echo =========================================
    echo [SUCCESS] Upload completed!
    echo =========================================
    echo.
    echo Next steps:
    echo 1. Visit your GitHub repository
    echo 2. Click Actions tab
    echo 3. Check build progress
    echo 4. Download AppOptWebUI after build completes
    echo.
    echo For details, see: GitHub快速开始.md
    echo.
) else (
    echo.
    echo =========================================
    echo [FAILED] Upload failed
    echo =========================================
    echo.
    echo Possible reasons:
    echo 1. Wrong repository URL
    echo 2. No permission
    echo 3. Network issue
    echo 4. Need Personal Access Token
    echo.
    echo Please check error message or use web upload
    echo Details: GitHub快速开始.md
    echo.
)

pause
