@echo off
title AegisOS Native Build Toolchain
echo ========================================================================
echo    *   AEGIS OS - NATIVE C BUILD TOOLCHAIN                      *
echo ========================================================================
echo.

where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] GCC compiler not found in PATH. Please install MinGW or GCC.
    exit /b 1
)

echo [1/2] Compiling AegisOS Native CLI Simulator (aegis_sim.exe)...
gcc -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -D__USE_MINGW_ANSI_STDIO=1 -Iinclude -o aegis_sim.exe simulator/native/aegis_cli_sim.c arch/x86_64/gdt.c arch/x86_64/idt.c arch/x86_64/context_switch.c kernel/memory.c kernel/process.c kernel/scheduler.c kernel/interrupt.c kernel/filesystem.c kernel/driver.c security/crypto_engine.c security/aegis_shield.c security/aegis_guard.c security/zero_trust_ac.c common/crypto.c filesystem/file_ops.c filesystem/file_indexing.c filesystem/file_encryption.c services/checkpoint_system.c services/system_troubleshoot.c services/task_optimizer.c services/neural_activity_monitor.c services/voice_commands.c services/command_palette.c userland/aegis_ui.c userland/aegis_ui_theme.c userland/aegis_shell.c
if %errorlevel% neq 0 (
    echo [ERROR] Failed to compile aegis_sim.exe.
    exit /b 1
)
echo [OK] aegis_sim.exe built successfully.

echo [2/2] Compiling AegisOS Test Suite (aegis_tests.exe)...
gcc -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -D__USE_MINGW_ANSI_STDIO=1 -Iinclude -Itests -o aegis_tests.exe tests/test_main.c tests/test_kernel.c tests/test_security.c tests/test_drivers.c tests/test_filesystem.c tests/test_devapi.c tests/test_integration.c tests/test_profiler.c kernel/memory.c kernel/process.c kernel/scheduler.c kernel/interrupt.c kernel/filesystem.c kernel/driver.c kernel/profiler.c kernel/syscall_gate.c kernel/ipc_bus.c kernel/event_system.c kernel/boot_params.c security/crypto_engine.c security/aegis_shield.c security/aegis_guard.c security/zero_trust_ac.c security/microkernel.c security/secure_boot.c security/hypervisor_protection.c security/immutable_partitions.c security/syscall_gatekeeper.c security/app_permissions.c drivers/driver_manager.c drivers/gpu_driver.c drivers/input_driver.c drivers/network_driver.c drivers/sensor_driver.c drivers/storage_driver.c filesystem/file_ops.c filesystem/file_indexing.c filesystem/file_encryption.c filesystem/journaling.c filesystem/metadata_engine.c filesystem/self_healing.c filesystem/snapshot_versioning.c devapi/core_api.c devapi/fs_api.c devapi/crypto_api.c devapi/network_api.c devapi/container_api.c devapi/sensor_api.c devapi/ui_api.c common/crypto.c arch/x86_64/gdt.c arch/x86_64/idt.c arch/x86_64/context_switch.c
if %errorlevel% neq 0 (
    echo [ERROR] Failed to compile aegis_tests.exe.
    exit /b 1
)
echo [OK] aegis_tests.exe built successfully.

echo.
echo ========================================================================
echo    * ALL AEGISOS NATIVE BINARIES BUILT CLEANLY WITHOUT ERRORS *
echo ========================================================================
