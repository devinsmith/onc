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

#include "app.h"
#include "network.h"

/* In order to use the WSAAsync* functions that Winsock offers we need to be
 * able to associate a window (HWND) to the asynchronous event so that
 * Windows can notify that window (through the window procedure) that the
 * event has completed. The way we do that is using the hidden window approach
 * that wxWidgets also uses.
 *
 * In the future maybe we can take a look at how FLTK or FOX toolkit handles
 * sockets.
 *
 * Also we may want to incorporate "Tiny Async DNS" library so that we don't
 * need to spin up a seperate thread for DNS lookups. */

static int net_initialized = 0;

static LRESULT CALLBACK
net_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  struct net_ctx *ctx;

  ctx = (struct net_ctx *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  if (msg == WM_CREATE) {
    CREATESTRUCT *lpcs;

    lpcs = (CREATESTRUCT *)lParam;
    ctx = (struct net_ctx *)lpcs->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ctx);

    ctx->win = hwnd;
    return 0;
  } else if (msg == SM_GETHOST) {
    if (ctx->resolv_cb != NULL) {
      ctx->resolv_cb(ctx, WSAGETASYNCERROR(lParam), ctx->resolv_extra);
    }
    return 0;
  } else if (msg == SM_ASYNC) {
    if (ctx->activity_cb != NULL) {
      ctx->activity_cb(ctx, WSAGETSELECTEVENT(lParam), ctx->activity_extra);
    }
    return 0;
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

static int
netwnd_register(const char *classname)
{
  WNDCLASS wc;

  HINSTANCE hInst = app_handle();

  memset(&wc, 0, sizeof(WNDCLASS));
  wc.lpfnWndProc = (WNDPROC)net_wnd_proc;
  wc.hInstance = hInst;
  wc.lpszClassName = classname;

  if (!RegisterClass(&wc)) {
    MessageBox(NULL, "This program requires a Windows based OS",
        "AppKit", MB_OK);
    return -1;
  }

  return 0;
}

static int
netwnd_hidden_window(struct net_ctx *ctx, const char *classname)
{
  HWND hwnd = CreateWindow(classname, NULL, 0, 0, 0, 0,
    0, (HWND) NULL, (HMENU)NULL, app_handle(), (LPVOID) ctx);

  if (hwnd == NULL)
    return -1;

  return 0;
}

int net_init(struct net_ctx *ctx, HWND notify)
{
  if (!net_initialized) {
    memset(ctx, 0, sizeof(struct net_ctx));

    // register and create a hidden window since Win32 async messages
    // tie in with the Win32 window message subsystem.
    netwnd_register("_xpui_netwnd");
    netwnd_hidden_window(ctx, "_xpui_netwnd");

    ctx->connected = FALSE;
    ctx->notify_window = notify;
    net_initialized = 1;
  }
  return 0;
}

void net_resolv(struct net_ctx *ctx, LPCSTR server)
{
  HANDLE hndlGetHost;     // Handle for WSAAsyncGetHostByName

  hndlGetHost = WSAAsyncGetHostByName(ctx->win, SM_GETHOST,
    server, ctx->bufHostEnt, MAXGETHOSTSTRUCT);
  if (hndlGetHost == 0) {
    MessageBox(ctx->notify_window,
      "Error initiating "
      "WSAAsyncGetHostByName()",
      "OpenNaken Chat", MB_OK);
  }
}

BOOL net_connect(struct net_ctx *ctx)
{
  struct sockaddr_in addr_chat;
  LPHOSTENT lpHostEntry;

  ctx->s = socket(AF_INET, SOCK_STREAM, 0);

  if (ctx->s == INVALID_SOCKET) {
    MessageBox(ctx->notify_window,
        "Debug: Socket connection failed",
        "ONC Info", MB_OK|MB_ICONERROR);
    return FALSE;
  }

  if (WSAAsyncSelect(ctx->s, ctx->win, SM_ASYNC,
      FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE)) {
    MessageBox(ctx->notify_window, "Error initiating WSAAsyncSelect()",
        "ONC Debug", MB_OK);
    return FALSE;
  }

  addr_chat.sin_port=htons(6666);
  addr_chat.sin_family=AF_INET;
  lpHostEntry = (LPHOSTENT)ctx->bufHostEnt;
  addr_chat.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

  if (connect(ctx->s, (struct sockaddr *)&addr_chat,sizeof(addr_chat)) != 0)
    return FALSE;
  return TRUE;
}

void net_resolv_cb(struct net_ctx *ctx,
    void (*resolv_cb)(struct net_ctx *ctx, int error, void *extra),
    void *extra)
{
  ctx->resolv_cb = resolv_cb;
  ctx->resolv_extra = extra;
}

void net_activity_cb(struct net_ctx *ctx,
    void (*activity_cb)(struct net_ctx *ctx, int event, void *extra),
    void *extra)
{
  ctx->activity_cb = activity_cb;
  ctx->activity_extra = extra;
}
