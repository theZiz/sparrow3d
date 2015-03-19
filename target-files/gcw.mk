# === Target file for the GCW. Use it with "make TARGET=gcw" ===

CC = /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
FLAGS = -DMOBILE_DEVICE -DGCW $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES) 
SDL = -I/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include
LIB = -L/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/lib -Wl,-rpath=/opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/lib
AR = /opt/gcw0-toolchain/usr/bin/mipsel-linux-ar
RANLIB = /opt/gcw0-toolchain/usr/bin/mipsel-linux-ranlib
