# === Target file for windows x86. Use it with "make TARGET=win32" ===
# Win32 SDL based on this tutorial: http://icculus.org/~dolson/sdl/
CPP = /usr/bin/i586-mingw32msvc-gcc -DMOBILE_DEVICE -DWIN32 -DX86CPU -DBUILDING_DLL $(GENERAL_TWEAKS) -Dmain=SDL_main -lmingw32 -lSDLmain -lSDL -mwindows
SDL = -I/opt/win32/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/win32/include -I/usr/i586-mingw32msvc/include
LIB = -L/opt/win32/lib -L/usr/i586-mingw32msvc/lib -Wl,-rpath=/opt/win32/lib -Wl,-rpath=/usr/i586-mingw32msvc/lib

SPARROW3D_LIB = sparrow3d.dll
SPARROWNET_LIB = sparrowNet.dll
SPARROWSOUND_LIB = sparrowSound.dll
SUFFIX = .exe
