# === Target file for the Orignal gp2x firmware. Use it with "make TARGET=gp2x" ===

CC = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc
FLAGS = -DMOBILE_DEVICE -DGP2X -DREALGP2X -DF100 -DBLACK_MAGIC -DUNSAVE_MAGIC -DDO_NOT_USE_DELAY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC += -Wl,-Bstatic -lSDL -lSDL_net -lm -Wl,-Bdynamic
DYNAMIC = -lSDL_image -lSDL_mixer -lSDL_ttf -lfreetype -lpng -lz -ljpeg
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
AR = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-ar
RANLIB = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-ranlib
