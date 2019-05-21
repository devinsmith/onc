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

#ifndef XPWINDOW_H
#define XPWINDOW_H

namespace XP {

struct WindowListElement {
  class Window *wnd;
  int expand;
  WindowListElement *next, *prev;
};

#define EXPAND_Y -1
#define EXPAND_X -2

class Window {
public:
  Window(Window *parent, const char *className, int w, int h);
  virtual ~Window();
  virtual void Show(void);
  virtual bool Create(DWORD exStyle, DWORD style, const char *title);

  HWND GetHWND() { return m_hwnd; }
  int GetHeight() { return m_height; }
  void SetHeight(int h) { m_height = h; }
protected:
  HWND m_hwnd;
  const char *className_;

  int m_width;
  int m_height;
  Window *parent_;
};

} // end of namespace XP

#endif /* XPWINDOW_H */
