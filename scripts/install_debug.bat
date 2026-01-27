@echo off
setlocal enabledelayedexpansion

:: ==============================================================================
:: EventReport 项目构建与安装脚本 (Debug 模式)
:: 
:: 功能：自动完成 CMake 配置、项目编译以及安装到指定目录
:: ==============================================================================

:: [步骤] 获取脚本所在目录，用于定位项目根目录
set SCRIPT_DIR=%~dp0

:: [步骤] 获取项目根目录 (脚本所在目录的上一级)
for %%i in ("%SCRIPT_DIR%..") do set PROJECT_ROOT=%%~fi

:: [步骤] 设置构建目录和安装目录路径
set BUILD_DIR=%PROJECT_ROOT%\build
set INSTALL_DIR=%PROJECT_ROOT%\install\debug

echo ========================================
echo   EventReport Build and Install Script
echo ========================================

:: [步骤] 检查并创建构建目录，确保 CMake 有输出位置
if not exist "%BUILD_DIR%" (
    echo [INFO] Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

echo.
echo [1/3] Configuring project with CMake...
:: [说明] -S 指定源码根目录，-B 指定构建中间产物输出目录，-DCMAKE_INSTALL_PREFIX 指定安装路径
cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"

:: [校验] 检查 CMake 配置阶段是否成功
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Building project (Debug mode)...
:: [说明] 使用 --build 调用底层构建工具（如 MSBuild），--config 指定编译模式
cmake --build "%BUILD_DIR%" --config Debug

:: [校验] 检查编译阶段是否成功
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Installing project to: %INSTALL_DIR%
:: [说明] 执行安装逻辑，将库、头文件等拷贝到 INSTALL_DIR
cmake --install "%BUILD_DIR%" --config Debug

:: [校验] 检查安装阶段是否成功
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Installation failed.
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo   Successfully installed to: %INSTALL_DIR%
echo ========================================
