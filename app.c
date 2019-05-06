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
#include <commctrl.h>

#include "app.h"
#include "font.h"

static HMODULE hRichEdit;
static HINSTANCE g_hInst;
static HACCEL hAccelTable = NULL;

static BOOL
InitWS2(void)
{
  int           Error;              // catches return value of WSAStartup
  WSADATA       WsaData;            // receives data from WSAStartup
  BOOL          ReturnValue = TRUE; // return value flag

  // Start WinSock 2.  If it fails, we don't need to call
  // WSACleanup().
  Error = WSAStartup(MAKEWORD(2, 0), &WsaData);
  if (Error) {
    MessageBox(NULL, "Could not find high enough version of WinSock",
      "Error", MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
    ReturnValue = FALSE;
  } else {
    // Now confirm that the WinSock 2 DLL supports the exact version
    // we want. If not, make sure to call WSACleanup().
    if (LOBYTE(WsaData.wVersion) != 2) {
      MessageBox(NULL, "Could not find the correct version of WinSock",
        "Error",  MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
      WSACleanup();
      ReturnValue = FALSE;
    }
  }
  return ReturnValue;
}

HINSTANCE app_handle(void)
{
  return g_hInst;
}

int
app_init(const char *appName)
{
  g_hInst = GetModuleHandle(NULL);

  /* Load Winsock */
  if (!InitWS2()) {
    return 0;
  }

  /* Load Richedit and common controls */
  hRichEdit = LoadLibrary("RICHED32.DLL");
  if (hRichEdit == NULL) {
    MessageBox(NULL, "Error: riched32.dll can't load", appName, MB_OK);
    return 0;
  }

  /* Ensure that the common control DLL is loaded. */
  InitCommonControls();

  xp_font_init();

  return 1;
}

void
app_set_accel(ACCEL *paccel, int num)
{
  hAccelTable = CreateAcceleratorTable(paccel, num);
}

void app_run(void)
{
  MSG msg;

  while (GetMessage(&msg, NULL, 0, 0)) {
    if (hAccelTable != NULL) {
      if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        continue;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void
app_close(void)
{
  WSACleanup();
  FreeLibrary(hRichEdit);
  if (hAccelTable != NULL) {
    DestroyAcceleratorTable(hAccelTable);
  }
}

