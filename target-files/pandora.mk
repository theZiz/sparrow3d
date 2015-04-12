# === Target file for the Pandora. Use it with "make TARGET=pandora" ===

#-DPANDORA_NEON -> does NEON magic. Not finished, but already sometimes faster

CC = /opt/pandora/arm-2011.09/bin/arm-none-linux-gnueabi-gcc
FLAGS = -DMOBILE_DEVICE -DPANDORA $(GENERAL_TWEAKS) -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp
SDL = `/opt/pandora/arm-2011.09/usr/bin/sdl-config --cflags`
INCLUDE = -I/opt/pandora/arm-2011.09/usr/include
DYNAMIC += -lpnd
LIB = -L/opt/pandora/arm-2011.09/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.09/usr/lib
AR = /opt/pandora/arm-2011.09/bin/arm-none-linux-gnueabi-ar
RANLIB = /opt/pandora/arm-2011.09/bin/arm-none-linux-gnueabi-ranlib
