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
AdjustWindowSize(window *wnd)
{
  RECT r;
  int left_pad, right_pad;
  int width;
  int max_height;
  int i;
  int padding = 2;
  int start = 0;

  GetClientRect(wnd->win, &r);

  left_pad = 5;
  right_pad = 5;

  max_height = r.bottom - r.top;
  width = (r.right - r.left);
  width -= left_pad;
  width -= right_pad;

  for (i = 0; i < wnd->n_children; i++) {
    window *child = wnd->children[i];

    if (child->expand != EXPAND_Y) {
      child->height = 24;
      max_height -= 24;
    }
    max_height -= padding; // Padding between elements.
  }

  for (i = 0; i < wnd->n_children; i++) {
    window *child = wnd->children[i];

    if (child->expand == EXPAND_Y) {
      child->height = max_height;
    }
    SetWindowPos(child->win, NULL, r.left + left_pad, start,
      width, child->height, SWP_NOZORDER);

    start += child->height + padding;
  }
}

static LRESULT CALLBACK
EditWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  window *wnd;

  wnd = (window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (message) {
  case WM_CHAR:
    if (wParam == VK_RETURN) {
      if (wnd->action_cb != NULL) {
        wnd->action_cb(wnd, wnd->action_extra);
      }
    }
  }

  return CallWindowProc(oldedit, hwnd, message, wParam, lParam);
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

window *
window_input_create(window *parent)
{
  window *ret;

  ret = calloc(1, sizeof(window));

  ret->win = CreateWindowEx(
      WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
      "Richedit", NULL, WS_BORDER | WS_CHILD | WS_GROUP | WS_TABSTOP |
      ES_LEFT | ES_AUTOHSCROLL,
      0, 0,
      0, 0,
      parent->win,
      NULL,
      app_handle(),
      NULL);

  if (ret->win == NULL)
    return NULL;

  // Since we can't capture WM_CREATE on the richedit control,
  // we'll set the userdata here so it'll always be available.
  SetWindowLongPtr(ret->win, GWLP_USERDATA, (LONG_PTR)ret);
  oldedit = (WNDPROC)SetWindowLongPtr(ret->win, GWLP_WNDPROC,
      (LONG_PTR)EditWndProc);

  SendMessage(ret->win, EM_SETBKGNDCOLOR, FALSE, RGB(0, 0, 0));
  SetupEditFont(ret->win);

  return ret;
}

window *
window_multi_create(window *parent)
{
  window *ret;

  ret = calloc(1, sizeof(window));

  ret->win = CreateWindowEx(
      WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
      TEXT("Richedit"), NULL, WS_BORDER | WS_CHILD | WS_GROUP |
      WS_TABSTOP | ES_LEFT | ES_MULTILINE | WS_VSCROLL | ES_READONLY |
      ES_AUTOVSCROLL,
      0, 0,
      0, 0,
      parent->win,
      NULL,
      app_handle(),
      NULL);

  if (ret->win == NULL)
    return NULL;

  SendMessage(ret->win, EM_SETBKGNDCOLOR, FALSE, RGB(0, 0, 0));
  SetupEditFont(ret->win);

  return ret;
}

static LRESULT CALLBACK
MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  window *wnd;
  CREATESTRUCT *lpcs;

  wnd = (window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  switch (msg) {
  case WM_CREATE:
    lpcs = (CREATESTRUCT*)lParam;
    wnd = (window *)lpcs->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wnd);

    wnd->win = hwnd;
    break;
  case WM_SIZE:
    AdjustWindowSize(wnd);
    break;
  case WM_COMMAND:
    if (wnd->menu_cb != NULL) {
      wnd->menu_cb(wnd, (int)(LOWORD(wParam)));
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

int window_register(const char *classname, const char *icon)
{
  WNDCLASS wc;

  HINSTANCE hInst = app_handle();

  memset(&wc, 0, sizeof(WNDCLASS));
  wc.lpfnWndProc = (WNDPROC)MainWndProc;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wc.hInstance = hInst;
  wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
  wc.lpszClassName = classname;
  wc.lpszMenuName = MAKEINTRESOURCE(IDMAINMENU);
  wc.hIcon = LoadIcon(hInst, icon);
  if (!RegisterClass(&wc)) {
    return 0;
  }

  return 1;
}

void window_layout(window *wnd)
{
  AdjustWindowSize(wnd);

  // Tell app to repaint itself.
  InvalidateRect(wnd->win, NULL, TRUE);
}

window *window_create(const char *className, const char *title)
{
  window *ret;

  ret = calloc(1, sizeof(window));

  ret->win = CreateWindow(className, title,
    WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS |
    WS_CLIPCHILDREN | WS_BORDER | WS_DLGFRAME | WS_OVERLAPPEDWINDOW |
    WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    CW_USEDEFAULT,
    0,
    CW_USEDEFAULT,
    0,
    NULL,
    NULL,
    app_handle(),
    ret);

  return ret;
}

void window_show(window *wnd)
{
  int i;

  ShowWindow(wnd->win, SW_SHOW);
  UpdateWindow(wnd->win);

  // Handle children
  for (i = 0; i < wnd->n_children; i++) {
    window *child = wnd->children[i];

    ShowWindow(child->win, SW_SHOW);
    UpdateWindow(child->win);
  }
}

void window_add_child(window *parent, window *child, int expand)
{
  window **children;

  children = realloc(parent->children, sizeof(window *) *
      (parent->n_children + 1));

  child->expand = expand; // Poor layout.

  children[parent->n_children] = child;
  parent->n_children++;
  parent->children = children;
}

void
window_set_action_cb(window *wnd,
    void (*actioncb)(window *win, void *extra), void *extra)
{
  wnd->action_cb = actioncb;
  wnd->action_extra = extra;
}

void
window_set_menu_cb(window *wnd, void (*menucb)(window *win, int id))
{
  wnd->menu_cb = menucb;
}
