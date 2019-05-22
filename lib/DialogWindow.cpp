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

#include <windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "app.h"
#include "DialogWindow.h"

static const char *dlgClassName = "xpDialogClass";

namespace XP {

bool DialogWindow::init_ = false;

DialogWindow::DialogWindow(const Window *parent) :
  CompositeFrame(parent, dlgClassName)
{
  if (!init_) {
    Init();
    init_ = true;
  }
}

DialogWindow::~DialogWindow()
{
}

LRESULT CALLBACK
DialogWindow::WndProcStub(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  DialogWindow *wnd;
  CREATESTRUCT *lpcs;

  wnd = (DialogWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (wnd) {
    return wnd->WndProc(msg, wParam, lParam);
  } else {
    if (msg == WM_CREATE) {
      lpcs = (CREATESTRUCT*)lParam;
      wnd = (DialogWindow *)lpcs->lpCreateParams;
      SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wnd);

      wnd->m_hwnd = hwnd;
      return wnd->WndProc(msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

LRESULT CALLBACK
DialogWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_CLOSE:
    EnableWindow(parent_->GetHWND(), TRUE);
    SetActiveWindow(parent_->GetHWND());
    DestroyWindow(m_hwnd);
    break;
  }
  return DefWindowProc(m_hwnd, msg, wParam, lParam);
}


void DialogWindow::Init()
{
  WNDCLASSEX wc = {};
  wc.cbSize           = sizeof(WNDCLASSEX);
  wc.lpfnWndProc      = (WNDPROC) DialogWindow::WndProcStub;
  wc.hInstance        = app_handle();
  wc.hbrBackground    = GetSysColorBrush(COLOR_3DFACE);
  wc.lpszClassName    = dlgClassName;
  RegisterClassEx(&wc);
}

void DialogWindow::Run()
{
  EnableWindow(parent_->GetHWND(), FALSE);

  MSG msg;

  while (IsWindow(m_hwnd)) {
    if (!GetMessage(&msg, NULL, 0, 0)) {
      PostQuitMessage(0); // quit out of next event loop
      break;
    }
    if (!IsDialogMessage(m_hwnd, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

bool DialogWindow::Create(const char *title, int x, int y)
{
  RECT r;
  POINT xy;

  GetClientRect(parent_->GetHWND(), &r);
  xy.x = r.left;
  xy.y = r.top;
  ClientToScreen(parent_->GetHWND(), &xy);

  bool ret = Window::Create(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST |
      WS_EX_WINDOWEDGE, WS_SYSMENU | WS_CAPTION, title);

  if (ret) {
    MoveWindow(m_hwnd, xy.x + x, xy.y + y, 500, 500, TRUE);
  }

  return ret;
}

} // namespace XP
