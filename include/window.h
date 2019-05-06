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

struct WindowListElement {
  class Window *wnd;
  int expand;
  WindowListElement *next, *prev;
};

#define EXPAND_Y -1
#define EXPAND_X -2

class Window {
public:
  Window(Window *parent, const char *className);
  virtual ~Window();
  virtual void Show(void);
  virtual bool Create(DWORD exStyle, DWORD style, const char *title);

  HWND GetHWND() { return m_hwnd; }
  int GetHeight() { return height; }
  void SetHeight(int h) { height = h; }
protected:
  HWND m_hwnd;
  const char *className_;

  int height;
  Window *parent_;
};

class MainWindow : public Window {
public:
  MainWindow(const char *className);
  virtual ~MainWindow();
  virtual void Show(void);
  bool Create(const char *title);
  bool Register(const char *icon, const char *menu);
  void add_child(Window *child, int expand);
  void layout(void);
  void set_menu_cb(void (*menucb)(MainWindow *win, int id));
protected:
  static LRESULT CALLBACK WndProcStub(HWND hwnd, UINT msg,
      WPARAM wParam, LPARAM lParam);
  LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
private:
  void AdjustWindowSize();

  WindowListElement *_wlist, *_wtail;

  void (*menu_cb)(MainWindow *win, int id);
};

class EditWindow : public Window {
public:
  EditWindow(Window *parent, bool multiLine);
  virtual ~EditWindow();

  virtual bool Create();

  void set_action_cb(void (*actioncb)(EditWindow *win, void *extra),
      void *extra);
protected:
  static LRESULT CALLBACK WndProcStub(HWND hwnd, UINT msg,
      WPARAM wParam, LPARAM lParam);
  LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
private:
  bool multiLine_;

  void (*action_cb)(EditWindow *win, void *extra);
  void *action_extra;
};

#endif /* XPWINDOW_H */
