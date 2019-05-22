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
#include "CompositeFrame.h"

namespace XP {

void
CompositeFrame::AdjustWindowSize()
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

void CompositeFrame::Show(void)
{
  WindowListElement *ptr;

  Window::Show(); // show myself.

  // Handle children
  for (ptr = _wlist; ptr != NULL; ptr = ptr->next) {
    Window *child = ptr->wnd;

    child->Show();
  }
}

CompositeFrame::CompositeFrame() :
  Window(NULL, "xpMainWndClass", CW_USEDEFAULT, CW_USEDEFAULT),
  _wlist{NULL}, _wtail{NULL}
{
}

CompositeFrame::~CompositeFrame()
{
}

void CompositeFrame::add_child(Window *child, int expand)
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

void CompositeFrame::layout(void)
{
  AdjustWindowSize();

  // Tell app to repaint itself.
  InvalidateRect(m_hwnd, NULL, TRUE);
}

} // End of namespace XP
