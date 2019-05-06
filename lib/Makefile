# Makefile for libwinui

.PHONY: all clean

C_SRCS = app.c font.c network.c
CXX_SRCS = EditWindow.cpp Window.cpp

OBJS = $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
DEPS = $(C_SRCS:.c=.d) $(CXX_SRCS:.cpp=.d)

CC = gcc
CXX = g++

# Dependencies
# On Mingw if using c++ link statically to libraries
# -static-libgcc -static-libstdc++
DEP_INCLUDES =
DEP_LIBS = -lws2_32 -lcomctl32 -lcomdlg32

CFLAGS = -Wall -O2 -I. -I../include

LIB = libwinui
STATIC_LIB = $(LIB).a

all: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	del /f $(STATIC_LIB) 2>NUL
	ar cru $(STATIC_LIB) $(OBJS)
	ranlib $(STATIC_LIB)

$(EXE): $(OBJS)
	$(CXX) -mwindows -static $(CFLAGS) -o $(EXE) $(OBJS) $(DEP_LFLAGS) $(DEP_LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $(DEP_INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

.c.o:
	$(CC) $(CFLAGS) $(DEP_INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

clean:
	del /f $(OBJS) 2>NUL
	del /f $(STATIC_LIB) 2>NUL
	del /f $(DEPS) 2>NUL

# Include automatically generated dependency files
-include $(DEPS)
