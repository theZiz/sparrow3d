# === Target file for the Wiz. Use it with "make TARGET=wiz" ===

CC = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc
FLAGS = -DMOBILE_DEVICE -DGP2X -DWIZ -DDOUBLEBUFFERING_BLIT_AND_FLIP -DSP_SURFACE_FLAGS=SDL_SWSURFACE $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC += -Wl,-Bstatic -lSDL_image -lpng -ljpeg -lz -lSDL_net -Wl,-Bdynamic
DYNAMIC = -lSDL_mixer -lSDL_ttf -lSDL -lm
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
AR = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-ar
RANLIB = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-ranlib
