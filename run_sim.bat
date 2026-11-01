@echo off
title AegisOS Simulator Launcher
echo ========================================================================
echo    *   AEGIS OS - HYBRID SECURE INTELLIGENCE OPERATING SYSTEM   *
echo    *   Kernel v2.0-Alpha | Dual-Kernel Hybrid | AI Self-Healing  *
echo ========================================================================
echo.
echo Choose simulator mode:
echo   [1] Launch Interactive Web GUI Desktop Simulator (Recommended)
echo   [2] Launch Native C CLI Simulator (Interactive Console)
echo   [3] Run Automated 8-Point Architecture Demo
echo   [4] Run Full Test Suite (84 Core Integration Tests)
echo.
set /p choice="Enter choice [1-4] (default: 1): "

if "%choice%"=="" set choice=1
if "%choice%"=="1" goto web_sim
if "%choice%"=="2" goto cli_sim
if "%choice%"=="3" goto demo_sim
if "%choice%"=="4" goto test_suite
goto web_sim

:web_sim
echo.
echo Starting AegisOS Web Simulator Server on http://localhost:3000 ...
start "" http://localhost:3000
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
echo Running AegisOS Unit and Integration Tests...
gcc -Iinclude -Itests tests/test_main.c tests/test_phase7_security.c tests/test_phase8_ui.c tests/test_phase9_advanced.c tests/test_implementation_needed.c kernel/memory.c kernel/scheduler.c kernel/interrupt.c kernel/ipc.c kernel/process.c kernel/syscall_gate.c kernel/ipc_bus.c kernel/profiler.c kernel/device_tree.c kernel/boot_params.c kernel/advanced_features.c security/crypto_engine.c security/microkernel.c security/aegis_guard.c security/aegis_shield.c security/zero_trust_ac.c security/app_permissions.c userland/aegis_ui.c userland/aegis_ui_theme.c userland/aegis_shell.c services/command_palette.c services/neural_activity_monitor.c services/system_troubleshoot.c services/task_optimizer.c services/checkpoint_system.c services/voice_commands.c filesystem/file_ops.c -o tests/aegis_test_runner.exe
tests\aegis_test_runner.exe
pause
goto end

:end
