# onc

onc is a chat client with a GUI for Michael Kohn's Nakenchat (www.naken.cc).
It was originally written in 2000 against the Win32 API. It should run on
Windows 98 and higher.

![Screenshot](onc.png)

# Building
On Windows you are expected to use mingw under the default cmd.exe console
host. Make sure to set your path to the mingw bin directory first. For example:

```
set PATH=%PATH%;d:\mingw64\bin

mingw32-make
```

# Code structure

The eventual goal is to seperate certain functionality into a seperate library
that can be reused with other apps, however this is not possible yet.

```
Library files:
  app.c/app.h - handles overall application state.
  network.c/network.h - async Winsock networking.
  window.c/window.h - Win32 window helper routines.
```

```
App files:
   about.c - "Help->About" dialog
   cfg.c - INI configuration.
   newdlg.c - "File->New" dialog.
   onc.c - main application
   opennaken.rc - Win32 resources
   prop.c - "File->Properties" dialog.
```
