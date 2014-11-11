# === Target file for the Wiz. Use it with "make TARGET=wiz" ===

CC = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc
FLAGS = -DMOBILE_DEVICE -DGP2X -DWIZ $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC += -Wl,-Bstatic -lpng -lSDL_net -Wl,-Bdynamic
DYNAMIC = -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
AR = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-ar
