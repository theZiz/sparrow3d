# === Target file for the Caanoo. Use it with "make TARGET=caanoo" ===

CC = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc
FLAGS = -DMOBILE_DEVICE -DCAANOO -DDOUBLEBUFFERING_BLIT $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
AR = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-ar
RANLIB = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-ranlib
