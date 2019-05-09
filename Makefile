# Makefile for onc

.PHONY: all clean

C_SRCS = about.c cfg.c newdlg.c prop.c
CXX_SRCS = onc.cpp

OBJS = $(C_SRCS:.c=.o) opennakenres.o $(CXX_SRCS:.cpp=.o)
DEPS = $(C_SRCS:.c=.d) $(CXX_SRCS:.cpp=.d)

CC = gcc
CXX = g++
RM = del /f 2>NUL
WINDRES = windres

# if no platform defined.
ifeq ($(PLATFORM),)
ifeq ($(OS),Windows_NT)
  HOST=
  CC = gcc
  CXX = g++
else
  HOST=
  RM = rm -f
  CC = x86_64-w64-mingw32-gcc
  CXX = x86_64-w64-mingw32-g++
  WINDRES = x86_64-w64-mingw32-windres
endif
endif

# Dependencies
# On Mingw if using c++ link statically to libraries
# -static-libgcc -static-libstdc++
DEP_INCLUDES = -Iinclude
DEP_LIBS = -Llib -lwinui -lws2_32 -lcomctl32 -lcomdlg32

CFLAGS = -Wall -O2 -I.

EXE = onc.exe

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -mwindows -static $(CFLAGS) -o $(EXE) $(OBJS) $(DEP_LFLAGS) $(DEP_LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $(DEP_INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

.c.o:
	$(CC) $(CFLAGS) $(DEP_INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

opennakenres.o: opennaken.rc opennakenres.h
	$(WINDRES) -o $@ opennaken.rc

clean:
	$(RM) $(OBJS)
	$(RM) $(EXE)
	$(RM) $(DEPS)

# Include automatically generated dependency files
-include $(DEPS)
