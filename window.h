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

#ifdef __cplusplus
extern "C" {
#endif

struct window;
typedef struct window window;

#define EXPAND_Y -1
#define EXPAND_X -2

struct window {
  HWND win;

  int expand;
  int height;

  void (*action_cb)(window *win, void *extra);
  void *action_extra;

  void (*menu_cb)(window *win, int id);

  // Children
  size_t n_children;
  window **children;
};


int window_register(const char *className, const char *text);
void window_show(window *wnd);
window *window_create(const char *className, const char *title);
window *window_input_create(window *parent);
window *window_multi_create(window *parent);
void window_layout(window *wnd);
void window_add_child(window *parent, window *child, int expand);
void window_set_menu_cb(window *wnd, void (*menucb)(window *win, int id));
void window_set_action_cb(window *wnd,
    void (*actioncb)(window *win, void *extra), void *extra);

#ifdef __cplusplus
}
#endif

#endif /* XPWINDOW_H */
