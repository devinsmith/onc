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
#include "Window.h"

namespace XP {

Window::Window(const Window *parent, const char *className, int w, int h) :
  m_hwnd{NULL}, className_{className}, m_width{w}, m_height{h}, parent_{parent}
{
}

Window::~Window()
{
}

void Window::Show(void)
{
  ShowWindow(m_hwnd, SW_SHOW);
  UpdateWindow(m_hwnd);
}

bool Window::Create(DWORD exStyle, DWORD style, const char *title)
{
  m_hwnd = CreateWindowEx(exStyle, className_, title, style,
    CW_USEDEFAULT,
    0,
    m_width,
    m_height,
    parent_ != NULL ? parent_->m_hwnd : NULL,
    NULL,
    app_handle(),
    this);

  if (m_hwnd != NULL)
    return true;

  return false;
}

} // End of namespace XP
