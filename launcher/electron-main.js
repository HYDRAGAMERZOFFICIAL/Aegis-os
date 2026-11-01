'use strict';
const { app, BrowserWindow, shell } = require('electron');
const path = require('path');

// The game runs the existing Aegis simulator locally; it never exposes a remote server.
process.env.PORT = process.env.AEGIS_PORT || '3210';
const server = require('../simulator/server');

let mainWindow;
const gameUrl = () => `http://127.0.0.1:${server.address().port}/game`;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1440,
    height: 920,
    minWidth: 1024,
    minHeight: 700,
    show: false,
    backgroundColor: '#07111b',
    title: 'AegisOS',
    autoHideMenuBar: true,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: false,
      contextIsolation: true,
      sandbox: true,
      webSecurity: true
    }
  });
  mainWindow.webContents.setWindowOpenHandler(({ url }) => {
    if (/^https:\/\//i.test(url)) shell.openExternal(url);
    return { action: 'deny' };
  });
  mainWindow.once('ready-to-show', () => mainWindow.show());
  mainWindow.loadURL(gameUrl());
  mainWindow.on('closed', () => { mainWindow = null; });
}

app.whenReady().then(() => {
  if (server.listening) createWindow();
  else server.once('listening', createWindow);
  app.on('activate', () => { if (BrowserWindow.getAllWindows().length === 0) createWindow(); });
});
app.on('window-all-closed', () => { if (process.platform !== 'darwin') app.quit(); });
app.on('before-quit', () => { if (server.listening) server.close(); });
