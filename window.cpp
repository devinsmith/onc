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
#include <richedit.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "app.h"
#include "network.h"
#include "opennakenres.h"
#include "window.h"

static WNDPROC oldedit; // Move to edit.c ?

void
MainWindow::AdjustWindowSize()
{
  RECT r;
  int left_pad, right_pad;
  int width;
  int max_height;
  int padding = 2;
  int start = 0;

  GetClientRect(win, &r);

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
EditWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case WM_CHAR:
    if (wParam == VK_RETURN) {
      if (action_cb != NULL) {
        action_cb(this, action_extra);
      }
    }
  }

  return CallWindowProc(oldedit, win, message, wParam, lParam);
}

static void SetupEditFont(HWND hWnd)
{
  CHARFORMAT fmt;

  fmt.cbSize = sizeof(CHARFORMAT);

  // Get existing character format.
  SendMessage(hWnd, EM_GETCHARFORMAT, FALSE, (LPARAM)&fmt);

  fmt.dwEffects = 0;
  fmt.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_BOLD;
  fmt.crTextColor = RGB(200, 200, 200); // Light gray.
  strcpy(fmt.szFaceName, "Verdana");
  fmt.yHeight = 165;

  if (SendMessage(hWnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &fmt) == 0)
    MessageBox(NULL, "SetupEditFont", "Circuit", MB_OK);
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

      wnd->win = hwnd;
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
    return DefWindowProc(win, msg, wParam, lParam);
  }
  return 0;
}

Window::Window(Window *parent, const char *className) : win{NULL},
  className_{className}, height{0}, parent_{parent}
{
}

Window::~Window()
{
}

void Window::Show(void)
{
  ShowWindow(win, SW_SHOW);
  UpdateWindow(win);
}

bool Window::Create(const char *title)
{
  win = CreateWindow(className_, title,
    WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS |
    WS_CLIPCHILDREN | WS_BORDER | WS_DLGFRAME | WS_OVERLAPPEDWINDOW |
    WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    CW_USEDEFAULT,
    0,
    CW_USEDEFAULT,
    0,
    parent_ != NULL ? parent_->win : NULL,
    NULL,
    app_handle(),
    this);

  if (win != NULL)
    return true;

  return false;
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

bool MainWindow::Register(const char *icon)
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
  wc.lpszMenuName = MAKEINTRESOURCE(IDMAINMENU);
  wc.hIcon = LoadIcon(hInst, icon);
  if (!RegisterClass(&wc)) {
    return false;
  }

  return true;
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
  InvalidateRect(win, NULL, TRUE);
}

EditWindow::EditWindow(Window *parent, const char *className, bool multiLine) :
  Window(parent, className), multiLine_{multiLine}
{
}

EditWindow::~EditWindow()
{
}

LRESULT CALLBACK
EditWindow::WndProcStub(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  EditWindow *wnd;

  wnd = (EditWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (wnd) {
    return wnd->WndProc(msg, wParam, lParam);
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool EditWindow::Create(const char *title)
{
  DWORD style = WS_BORDER | WS_CHILD | WS_GROUP | WS_TABSTOP | ES_LEFT;

  if (multiLine_)
    style |= ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_AUTOVSCROLL;
  else
    style |= ES_AUTOHSCROLL;


  win = CreateWindowEx(
      WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
      TEXT("Richedit"), NULL, style,
      0, 0,
      0, 0,
      parent_->GetHWND(),
      NULL,
      app_handle(),
      NULL);

  if (!multiLine_) {
    // Since we can't capture WM_CREATE on the richedit control,
    // we'll set the userdata here so it'll always be available.
    SetWindowLongPtr(win, GWLP_USERDATA, (LONG_PTR)this);
    oldedit = (WNDPROC)SetWindowLongPtr(win, GWLP_WNDPROC,
      (LONG_PTR)EditWindow::WndProcStub);
  }

  SendMessage(win, EM_SETBKGNDCOLOR, FALSE, RGB(0, 0, 0));
  SetupEditFont(win);

  return true;
}

void
EditWindow::set_action_cb(void (*actioncb)(EditWindow *win, void *extra),
    void *extra)
{
  action_cb = actioncb;
  action_extra = extra;
}
