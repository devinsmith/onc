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

#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

enum net_msg {
  SM_GETHOST = WM_USER+1,
  SM_ASYNC
};

struct net_ctx {
  BOOL connected;
  HWND notify_window;
  HWND win;
  SOCKET s;
  char bufHostEnt[MAXGETHOSTSTRUCT];

  void (*resolv_cb)(struct net_ctx *ctx, int error, void *extra);
  void *resolv_extra;

  void (*activity_cb)(struct net_ctx *ctx, int event, void *extra);
  void *activity_extra;
};

int net_init(struct net_ctx *ctx, HWND notify);

void net_resolv_cb(struct net_ctx *ctx,
    void (*resolv_cb)(struct net_ctx *ctx, int error, void *extra),
    void *extra);
void net_activity_cb(struct net_ctx *ctx,
    void (*resolv_cb)(struct net_ctx *ctx, int error, void *extra),
    void *extra);
void net_resolv(struct net_ctx *ctx, LPCSTR server);
BOOL net_connect(struct net_ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_H */
