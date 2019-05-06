# Makefile for onc

.PHONY: all clean

C_SRCS = about.c app.c cfg.c font.c network.c newdlg.c prop.c
CXX_SRCS = onc.cpp window.cpp

OBJS = $(C_SRCS:.c=.o) opennakenres.o $(CXX_SRCS:.cpp=.o)
DEPS = $(C_SRCS:.c=.d) $(CXX_SRCS:.cpp=.d)

CC = gcc
CXX = g++

# Dependencies
# On Mingw if using c++ link statically to libraries
# -static-libgcc -static-libstdc++
DEP_INCLUDES =
DEP_LIBS = -lws2_32 -lcomctl32 -lcomdlg32

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
	windres -o $@ opennaken.rc

clean:
	del /f $(OBJS) 2>NUL
	del /f $(EXE) 2>NUL
	del /f $(DEPS) 2>NUL

# Include automatically generated dependency files
-include $(DEPS)
