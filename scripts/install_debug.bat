@echo off
setlocal enabledelayedexpansion

:: 获取项目根目录 (脚本所在目录的上一级)
set SCRIPT_DIR=%~dp0
for %%i in ("%SCRIPT_DIR%..") do set PROJECT_ROOT=%%~fi

set BUILD_DIR=%PROJECT_ROOT%\build
set INSTALL_DIR=%PROJECT_ROOT%\install\debug

echo ========================================
echo   EventReport Build and Install (Debug)
echo ========================================

:: 检查并创建构建目录
if not exist "%BUILD_DIR%" (
    echo Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

echo.
echo [1/3] Configuring project with CMake...
:: -S 指向项目根目录，-B 指向构建目录
cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Building project (Debug mode)...
cmake --build "%BUILD_DIR%" --config Debug

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Installing project to: %INSTALL_DIR%
cmake --install "%BUILD_DIR%" --config Debug

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Installation failed.
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo   Successfully installed (Debug) to: %INSTALL_DIR%
echo ========================================
