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
#include "font.h"
#include "EditWindow.h"

namespace XP {

static WNDPROC oldedit; // Move to edit.c ?

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

  return CallWindowProc(oldedit, m_hwnd, message, wParam, lParam);
}

static void SetupEditFont(HWND hWnd)
{
  CHARFORMAT fmt;
  const struct xpFont *default_font = get_default_font();

  fmt.cbSize = sizeof(CHARFORMAT);

  // Get existing character format.
  SendMessage(hWnd, EM_GETCHARFORMAT, FALSE, (LPARAM)&fmt);

  fmt.dwEffects = 0;
  fmt.dwMask = CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_BOLD;
  fmt.crTextColor = RGB(200, 200, 200); // Light gray.
  strcpy(fmt.szFaceName, default_font->faceName);
  fmt.yHeight = default_font->ptSize * 20;

  if (SendMessage(hWnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &fmt) == 0)
    MessageBox(NULL, "SetupEditFont", "Circuit", MB_OK);
}

EditWindow::EditWindow(Window *parent, bool multiLine) :
  Window(parent, "RichEdit", 0, 0), multiLine_{multiLine}
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

bool EditWindow::Create()
{
  DWORD style = WS_BORDER | WS_CHILD | WS_GROUP | WS_TABSTOP | ES_LEFT;

  if (multiLine_)
    style |= ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_AUTOVSCROLL;
  else
    style |= ES_AUTOHSCROLL;

  Window::Create(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR,
      style, NULL);

  if (!multiLine_) {
    // Since we can't capture WM_CREATE on the richedit control,
    // we'll set the userdata here so it'll always be available.
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    oldedit = (WNDPROC)SetWindowLongPtr(m_hwnd, GWLP_WNDPROC,
      (LONG_PTR)EditWindow::WndProcStub);
  }

  SendMessage(m_hwnd, EM_SETBKGNDCOLOR, FALSE, RGB(0, 0, 0));
  SetupEditFont(m_hwnd);

  return true;
}

void
EditWindow::set_action_cb(void (*actioncb)(EditWindow *win, void *extra),
    void *extra)
{
  action_cb = actioncb;
  action_extra = extra;
}

} // End of namespace XP
