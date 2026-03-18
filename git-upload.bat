@echo off
REM Simple Git Upload Script
REM Author: Liu Shisan

echo =========================================
echo Upload to GitHub
echo =========================================
echo.

REM Check Git
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Git not found
    echo Please install: https://git-scm.com/download/win
    pause
    exit /b 1
)

echo [OK] Git found
git --version
echo.

REM Init if needed
if not exist .git (
    echo [STEP 1] Initialize Git...
    git init
    echo.
    
    echo Enter your GitHub repository URL:
    echo Example: https://github.com/username/AppOpt-8Gen2-Mi13.git
    echo.
    set /p REPO_URL=URL: 
    
    if "%REPO_URL%"=="" (
        echo [ERROR] URL cannot be empty
        pause
        exit /b 1
    )
    
    git remote add origin %REPO_URL%
    echo [OK] Remote added
    echo.
)

REM Add files
echo [STEP 2] Adding files...
git add .github/
git add webui/
git add README.md
git add .gitignore

echo [OK] Files added
echo.

REM Commit
echo [STEP 3] Commit...
git commit -m "Initial commit: AppOpt WebUI"

echo [OK] Committed
echo.

REM Push
echo [STEP 4] Push to GitHub...
echo.
echo NOTE: You may need to login
echo   Username: Your GitHub username
echo   Password: Personal Access Token (NOT your password)
echo.
echo How to get Token:
echo   GitHub -^> Settings -^> Developer settings
echo   -^> Personal access tokens -^> Generate new token
echo   -^> Check 'repo' -^> Generate -^> Copy token
echo.
pause

git push -u origin main

if %ERRORLEVEL% EQU 0 (
    echo.
    echo =========================================
    echo [SUCCESS] Upload completed!
    echo =========================================
    echo.
    echo Next: Go to your GitHub repo -^> Actions tab
    echo Wait 10 minutes for build to complete
    echo Download AppOptWebUI from Artifacts
    echo.
) else (
    echo.
    echo =========================================
    echo [FAILED] Upload failed
    echo =========================================
    echo.
    echo Try: git push -u origin master
    echo Or use web upload method
    echo.
)

pause
