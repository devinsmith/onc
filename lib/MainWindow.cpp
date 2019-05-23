/*
 * Copyright (c) 2000-2019 Devin Smith <devin@devinsmith.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "app.h"
#include "MainWindow.h"

namespace XP {

LRESULT CALLBACK
MainWindow::WndProcStub(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  MainWindow *wnd;
  CREATESTRUCT *lpcs;

  wnd = (MainWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (wnd) {
    return wnd->WndProc(msg, wParam, lParam);
  } else {
    if (msg == WM_CREATE) {
      lpcs = (CREATESTRUCT*)lParam;
      wnd = (MainWindow *)lpcs->lpCreateParams;
      SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wnd);

      wnd->m_hwnd = hwnd;
      return wnd->WndProc(msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

LRESULT CALLBACK
MainWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_SIZE:
    layout();
    break;
  case WM_COMMAND:
    OnMenuClick((int)(LOWORD(wParam)));
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(m_hwnd, msg, wParam, lParam);
  }
  return 0;
}

MainWindow::MainWindow() : CompositeFrame(NULL, "xpMainWndClass")
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::Register(const char *menu)
{
  WNDCLASS wc;

  HINSTANCE hInst = app_handle();

  memset(&wc, 0, sizeof(WNDCLASS));
  wc.lpfnWndProc = (WNDPROC)MainWindow::WndProcStub;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wc.hInstance = hInst;
  wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
  wc.lpszClassName = className_;
  wc.lpszMenuName = menu;
  wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
  if (!RegisterClass(&wc)) {
    return false;
  }

  return true;
}

bool MainWindow::Create(const char *title)
{
  return Window::Create(0,
    WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS |
    WS_CLIPCHILDREN | WS_BORDER | WS_DLGFRAME | WS_OVERLAPPEDWINDOW |
    WS_MINIMIZEBOX | WS_MAXIMIZEBOX, title);
}

} // End of namespace XP
