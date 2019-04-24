/* $Id: prop.c 2525 2009-04-25 22:43:32Z devin $ */

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

#include <windows.h>
#include <prsht.h>

#include "app.h"
#include "prop.h"
#include "opennakenres.h"

static INT_PTR _stdcall ProperDlg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {
	case WM_CLOSE:
		EndDialog(hwnd,0);
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			EndDialog(hwnd,1);
			return 1;
		case IDOK:
			EndDialog(hwnd, IDOK);
			 break;
		}
		break;
	}
	return 0;
}

static INT_PTR _stdcall ProperConnect(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {
	case WM_CLOSE:
		EndDialog(hwnd,0);
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			EndDialog(hwnd,1);
			return 1;
		case IDOK:
			EndDialog(hwnd, IDOK);
			break;
		}
		break;
	}
	return 0;
}

int propertydlg(HWND hwndOwner)
{
	HINSTANCE hInst = app_handle();

	PROPSHEETPAGE psp[2];
	PROPSHEETHEADER psh;
	psp[0].dwSize = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags = PSP_USETITLE;
	psp[0].hInstance = hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PROP);
	psp[0].pszIcon = NULL;
	psp[0].pfnDlgProc = ProperDlg;
	psp[0].pszTitle = "Styles";
	psp[0].lParam = 0;

	psp[1].dwSize = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags = PSP_USETITLE;
	psp[1].hInstance = hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_CONNECT);
	psp[1].pszIcon = NULL;
	psp[1].pfnDlgProc = ProperConnect;
	psp[1].pszTitle = "Connect";
	psp[1].lParam = 0;

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
	psh.hwndParent = hwndOwner;
	psh.hInstance = hInst;
	psh.pszIcon = NULL;
	psh.pszCaption = (LPSTR) "Properties";
	psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.ppsp = (LPCPROPSHEETPAGE) &psp;

	return (PropertySheet(&psh));
}

