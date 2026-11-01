@echo off
title AegisOS Simulator Launcher
echo ========================================================================
echo    *   AEGIS OS - HYBRID SECURE INTELLIGENCE OPERATING SYSTEM   *
echo    *   Kernel v2.0-Alpha [Dual-Kernel Hybrid] [AI Self-Healing]    *
echo ========================================================================
echo.

if "%1"=="web" goto web_sim
if "%1"=="cli" goto cli_sim
if "%1"=="demo" goto demo_sim
if "%1"=="test" goto test_suite

echo Choose simulator mode:
echo   [1] Launch Interactive Web GUI Desktop Simulator (Recommended)
echo   [2] Launch Native C CLI Simulator (Interactive Console)
echo   [3] Run Automated Architecture Demo
echo   [4] Run Full Test Suite
echo.
set choice=1
set /p choice="Enter choice [1-4] (default: 1): "

rem Extract first non-space character
set choice_char=%choice:~0,1%
if "%choice_char%"=="2" goto cli_sim
if "%choice_char%"=="3" goto demo_sim
if "%choice_char%"=="4" goto test_suite
goto web_sim

:web_sim
echo.
echo Launching AegisOS Web Simulator on http://localhost:3000 ...
start "" http://localhost:3000
netstat -ano | findstr :3000 | findstr LISTENING >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Web Simulator Server is already active on http://localhost:3000.
    echo Desktop environment opened in your browser.
    goto end
)
node "%~dp0simulator\server.js"
goto end

:cli_sim
echo.
echo Launching AegisOS Native C Simulator...
"%~dp0aegis_sim.exe"
goto end

:demo_sim
echo.
echo Running AegisOS Architecture Demo...
"%~dp0aegis_sim.exe" --demo
pause
goto end

:test_suite
echo.
echo Running AegisOS Integration Architecture Tests...
"%~dp0aegis_tests.exe"
pause
goto end

:end
