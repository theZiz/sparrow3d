# === Target file for open2x. use it with "make TARGET=open2x" ===

# CPP_LINK = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100 -DFAST_BUT_UGLY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
# CPP = /opt/open2x/wiz-toolchain-4.6.3-3/bin/arm-wiz-linux-gnu-gcc -mfpu=fpa -msoft-float -mcpu=arm920t -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100 -DFAST_BUT_UGLY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DREALGP2X -DF100 -DFAST_BUT_UGLY -DDO_NOT_USE_DELAY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags` 
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
