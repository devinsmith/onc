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

#ifndef XPEDITWINDOW_H
#define XPEDITWINDOW_H

#include "Window.h"

namespace XP {

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

} // end of namespace XP

#endif /* XPEDITWINDOW_H */
