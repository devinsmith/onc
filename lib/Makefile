# Makefile for libwinui

.PHONY: all clean

C_SRCS = app.c font.c network.c
CXX_SRCS = CompositeFrame.cpp DialogWindow.cpp EditWindow.cpp MainWindow.cpp \
	Window.cpp

OBJS = $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
DEPS = $(C_SRCS:.c=.d) $(CXX_SRCS:.cpp=.d)

CC = gcc
CXX = g++
RM = del /f 2>NUL

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
endif
endif


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
	$(RM) $(STATIC_LIB)
	ar cru $(STATIC_LIB) $(OBJS)
	ranlib $(STATIC_LIB)

$(EXE): $(OBJS)
	$(CXX) -mwindows -static $(CFLAGS) -o $(EXE) $(OBJS) $(DEP_LFLAGS) $(DEP_LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $(DEP_INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

.c.o:
	$(CC) $(CFLAGS) $(DEP_INCLUDES) -MMD -MP -MT $@ -o $@ -c $<

clean:
	$(RM) $(OBJS)
	$(RM) $(STATIC_LIB)
	$(RM) $(DEPS)

# Include automatically generated dependency files
-include $(DEPS)
