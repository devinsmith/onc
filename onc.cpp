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

/* Initial creation: Written Sun May 21 09:17:55 2000 */
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <stdarg.h>

#include "about.h"
#include "app.h"
#include "network.h"
#include "newdlg.h"
#include "onc.h"
#include "prop.h"
#include "EditWindow.h"
#include "MainWindow.h"
#include "Window.h"

using namespace XP;

const char *szMainWindowText = "OpenNaken";
const char *szMainWindowTitle = "OpenNaken v0.2";

struct net_ctx net_ctx;
struct new_dlg new_dlg;

static void
resolve_done(struct net_ctx *ctx, int error, void *extra)
{
  OncApp *app = (OncApp *)extra;

  SendMessage(app->m_display->GetHWND(), EM_REPLACESEL, FALSE,
      (LPARAM)((LPSTR)"Looking up host..\r\n"));
  if (error) {
    MessageBox(app->GetHWND(),
        "There was a problem resolving the server "
        "name address you entered. Please make sure that you are connected to "
        "the internet and try again.", szMainWindowText,
        MB_OK | MB_ICONERROR);
    return;
  }

  net_connect(&net_ctx);
}

void
DoStartupTransmit(MainWindow *wnd)
{
  int d;
  char textr[256] ;
  wsprintf(textr, ".n %s\r\n", new_dlg.username) ;
  d = send(net_ctx.s, textr, strlen(textr), 0);
  if (d == SOCKET_ERROR) {
    MessageBox(wnd->GetHWND(),
        "send() is having a problem, check your connection.",
        "OpenNaken Chat", MB_OK | MB_ICONERROR);
    closesocket(net_ctx.s);
  }
  wsprintf(textr, "%% has connected with OpenNaken (Win32) v.0.2\r\n");
  d = send(net_ctx.s, textr, strlen(textr), 0);
  if (d == SOCKET_ERROR) {
    MessageBox(wnd->GetHWND(), "send() is having a problem, check your connection.",
        "OpenNaken Chat", MB_OK|MB_ICONERROR);
    closesocket(net_ctx.s);
  }
}

void
Put(HWND wnd, const char *format, ...)
{
  va_list arglist;
  char output[2048];

  va_start(arglist, format);
  /* _vsnprintf produces smaller binaries than
   * vsnprintf. Don't ask me why */
  vsnprintf(output, sizeof(output), format, arglist);
#if !defined(__LCC__)
  /* lcc's warning system can't handle this I guess. */
  va_end(arglist);
#endif

  SendMessage(wnd, EM_REPLACESEL, FALSE, (LPARAM)output);
  SendMessage(wnd, EM_SCROLLCARET, 0, 0);
  SendMessage(wnd, EM_SETSEL, 0x7FFFFFFF, 0x7FFFFFFF);
}

void
HandleData(HWND hwndDisp, LPSTR lpBuf, int nBytesRead)
{
  static char input[2048];
  static int input_idx = 0;
  int i;

  /* We should really do operations in bulk and not operate
   * on a character at a time. Maybe in the next version. This
   * was faster to code */
  for (i = 0; i < nBytesRead; i++)
  {
    if (lpBuf[i] == '\r') continue;

    if (lpBuf[i] == '\n')
    {
      /* Display message to user */
      input[input_idx] = '\0';
      Put(hwndDisp, "%s\r\n", input);

      /* clear out our input buffer */
      input_idx = 0;
    }
    else
      input[input_idx++] = lpBuf[i];
  }
}

void
OncApp::OnMenuClick(int id)
{
  switch (id) {
  case IDM_ABOUT:
    about_dlg_show(app_handle(), GetHWND());
    break;
  case IDM_NEW:
    new_dlg.hInst = app_handle();
    new_dlg.parent = GetHWND();
    new_dlg.net = &net_ctx;
    new_dlg.username[0] = '\0';

    new_dlg_show(&new_dlg);
    break;
  case IDM_DISC:
    if (MessageBox(GetHWND(), "Are you sure you want to disconnect?",
          "OpenNaken Chat", MB_ICONINFORMATION | MB_YESNO) == IDYES) {
        closesocket(net_ctx.s);
        net_ctx.connected = FALSE;
    }
    break;
  case IDM_PROP:
    propertydlg(GetHWND());
    break;
  case IDM_HELP:
    MessageBox(GetHWND(), "Help coming soon!", "OpenNaken Chat",
        MB_ICONINFORMATION | MB_OK);
    break;
  case IDM_EXIT:
    PostMessage(GetHWND(), WM_CLOSE, 0, 0);
    break;
  }
}

static void
net_activity(struct net_ctx *ctx, int event, void *extra)
{
  int nBytesRead;
  char gbufRecv[512];     // the buffer
  OncApp *app = (OncApp *)extra;

  switch (event) {
  case FD_CONNECT:
    SendMessage(app->m_display->GetHWND(), EM_REPLACESEL, FALSE, (LPARAM)((LPSTR)"Connected!\r\n"));
    ctx->connected = TRUE;
    DoStartupTransmit(app);
    break;

  case FD_WRITE:
    SendMessage(app->m_display->GetHWND(), EM_REPLACESEL, FALSE, (LPARAM)((LPSTR)"Sending data!\r\n"));
    break;
  case FD_READ:
    nBytesRead = recv(ctx->s,
      gbufRecv,  // buffer
      sizeof(gbufRecv), // max len to read
      0);

    if (nBytesRead == 0) {
      // Connection has been closed.
      SendMessage(app->m_display->GetHWND(), EM_REPLACESEL, FALSE, (LPARAM)((LPSTR)"Your connection has been closed!\r\n"));
      break;
    }
    gbufRecv[nBytesRead] = '\0';
    HandleData(app->m_display->GetHWND(), gbufRecv, nBytesRead);
    break;
  }
}

static void
input_action_cb(EditWindow *input, void *extra)
{
  OncApp *app = (OncApp *)extra;
  char textr[256];
  int d;

  if (net_ctx.connected) {
    Edit_GetText(input->GetHWND(), textr, sizeof(textr));
    Edit_SetText(input->GetHWND(), "") ;
    wsprintf(textr, "%s \r\n", textr);
    d = send(net_ctx.s, textr, strlen(textr), 0);
    if (d == SOCKET_ERROR) {
      MessageBox(app->GetHWND(),
        "send() is having a problem, check your connection.",
        "OpenNaken Chat", MB_OK | MB_ICONERROR);
      closesocket(net_ctx.s);
      net_ctx.connected = FALSE;
    }
    return;
  } else {
    MessageBox(app->GetHWND(), "Please connect to a server first",
      "OpenNaken Chat", MB_OK | MB_ICONERROR);
  }
}

static void set_accelerators(void)
{
  ACCEL accel[1];

  accel[0].key = 'Q';
  accel[0].cmd = IDM_EXIT;
  accel[0].fVirt = FCONTROL | FVIRTKEY;

  app_set_accel(accel, 1);
}

int OncApp::CreateChildren()
{
  m_display = new EditWindow(this, true);
  m_input = new EditWindow(this, false);

  if (m_input == NULL || m_display == NULL)
    return 0;

  m_display->Create();
  m_input->Create();

  m_input->set_action_cb(input_action_cb, this);

  add_child(m_display, EXPAND_Y);
  add_child(m_input, 0);

  return 1;
}

int main(int argc, const char *argv[])
{
  if (!app_init(szMainWindowText)) {
    return 1;
  }

  set_accelerators();

  OncApp app;

  if (!app.Register(MAKEINTRESOURCE(IDMAINMENU))) {
    MessageBox(NULL, "Program failed to start due to class registration "
        "failure.", szMainWindowText, MB_OK | MB_ICONEXCLAMATION);
    return -1;
  }

  if (!app.Create(szMainWindowTitle)) {
    MessageBox(NULL, "Error creating main window",
        szMainWindowText, MB_OK);
    return -1;
  }

  // Initialize network layer.
  net_init(&net_ctx, app.GetHWND());
  // Register callbacks
  net_resolv_cb(&net_ctx, resolve_done, &app);
  net_activity_cb(&net_ctx, net_activity, &app);

  app.CreateChildren();
  app.layout();

  app.Show();

  app_run();
  app_close();
  return 0;
}

