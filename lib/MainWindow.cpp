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

void
MainWindow::AdjustWindowSize()
{
  RECT r;
  int left_pad, right_pad;
  int width;
  int max_height;
  int padding = 2;
  int start = 0;

  GetClientRect(m_hwnd, &r);

  left_pad = 5;
  right_pad = 5;

  max_height = r.bottom - r.top;
  width = (r.right - r.left);
  width -= left_pad;
  width -= right_pad;

  WindowListElement *ptr;

  for (ptr = _wlist; ptr != NULL; ptr = ptr->next) {
    Window *child = ptr->wnd;

    if (ptr->expand != EXPAND_Y) {
      child->SetHeight(24);
      max_height -= 24;
    }
    max_height -= padding; // Padding between elements.
  }

  for (ptr = _wlist; ptr != NULL; ptr = ptr->next) {
    Window *child = ptr->wnd;

    if (ptr->expand == EXPAND_Y) {
      child->SetHeight(max_height);
    }
    SetWindowPos(child->GetHWND(), NULL, r.left + left_pad, start,
      width, child->GetHeight(), SWP_NOZORDER);

    start += child->GetHeight() + padding;
  }
}

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
    AdjustWindowSize();
    break;
  case WM_COMMAND:
    if (menu_cb != NULL) {
      menu_cb(this, (int)(LOWORD(wParam)));
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(m_hwnd, msg, wParam, lParam);
  }
  return 0;
}

void MainWindow::Show(void)
{
  WindowListElement *ptr;

  Window::Show(); // show myself.

  // Handle children
  for (ptr = _wlist; ptr != NULL; ptr = ptr->next) {
    Window *child = ptr->wnd;

    child->Show();
  }
}

MainWindow::MainWindow(const char *className) : Window(NULL, className), 
  _wlist{NULL}, _wtail{NULL}, menu_cb{NULL}
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::Register(const char *icon, const char *menu)
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
  wc.hIcon = LoadIcon(hInst, icon);
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

void MainWindow::add_child(Window *child, int expand)
{
  WindowListElement *nw;

  nw = new WindowListElement;
  nw->wnd = child;
  nw->expand = expand;
  nw->next = NULL;
  nw->prev = NULL;

  // First element.
  if (_wlist == NULL) {
    _wlist = _wtail = nw;
  } else {
    _wtail->next = nw;
    nw->prev = _wtail;
    _wtail = nw;
  }
}

void MainWindow::set_menu_cb(void (*menucb)(MainWindow *win, int id))
{
  menu_cb = menucb;
}

void MainWindow::layout(void)
{
  AdjustWindowSize();

  // Tell app to repaint itself.
  InvalidateRect(m_hwnd, NULL, TRUE);
}

} // End of namespace XP
