@echo off
setlocal enabledelayedexpansion

:: 设置构建目录
set BUILD_DIR=build
:: 获取当前脚本所在目录作为项目根目录
set PROJECT_ROOT=%~dp0
set INSTALL_DIR=%PROJECT_ROOT%install

echo ========================================
echo   EventReport Build and Install Script
echo ========================================

:: 检查并创建构建目录
if not exist "%BUILD_DIR%" (
    echo Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

echo.
echo [1/3] Configuring project with CMake...
:: 强制设置安装路径为当前项目下的 install 目录
cmake -S . -B "%BUILD_DIR%" -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Building project (Release mode)...
cmake --build "%BUILD_DIR%" --config Release

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Installing project to: %INSTALL_DIR%
cmake --install "%BUILD_DIR%" --config Release

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Installation failed.
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo   Successfully installed to: %INSTALL_DIR%
echo ========================================
