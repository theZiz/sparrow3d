# === Target file for the Pandora. Use it with "make TARGET=pandora" ===

CPP = /opt/pandora/arm-2011.09/bin/arm-none-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DPANDORA $(GENERAL_TWEAKS) -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp
SDL = `/opt/pandora/arm-2011.09/usr/bin/sdl-config --cflags`
INCLUDE = -I/opt/pandora/arm-2011.09/usr/include
LIB = -L/opt/pandora/arm-2011.09/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.09/usr/lib -lpnd
