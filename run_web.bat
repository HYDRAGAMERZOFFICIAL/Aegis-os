@echo off
title AegisOS Web Simulator
echo ========================================================================
echo    *   AEGIS OS - HYBRID SECURE INTELLIGENCE OPERATING SYSTEM   *
echo    *   Interactive Web GUI Desktop Simulator                   *
echo ========================================================================
echo.
echo Starting Web Simulator Server...
start "" http://localhost:3000
node "%~dp0simulator\server.js"
