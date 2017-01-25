# === Target file for windows x86. Use it with "make TARGET=win32" ===
# Win32 SDL based on this tutorial: http://icculus.org/~dolson/sdl/

CC = /usr/bin/i686-w64-mingw32-gcc
FLAGS = $(GENERAL_TWEAKS) -DDESKTOP
LINK_FLAGS =  -Dmain=SDL_main -lmingw32 -lSDLmain -lSDL -mwindows
SDL = -I/opt/win32/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/win32/include -I/usr/i686-w64-mingw32/include
LIB = -L/opt/win32/lib -L/usr/i686-w64-mingw32/lib -Wl,-rpath=/opt/win32/lib -Wl,-rpath=/usr/i686-w64-mingw32/lib
AR = /usr/bin/i686-w64-mingw32-ar
RANLIB = /usr/bin/i686-w64-mingw32-ranlib

SPARROW3D_LIB = sparrow3d.dll
SPARROWNET_LIB = sparrowNet.dll
SPARROWSOUND_LIB = sparrowSound.dll
SPARROW3D_STATIC_LIB = libsparrow3d_static.a
SPARROWNET_STATIC_LIB = libsparrowNet_static.a
SPARROWSOUND_STATIC_LIB = libsparrowSound_static.a
SUFFIX = .exe
RDYNAMIC =
