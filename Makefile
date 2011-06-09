#==PC==
CPP = gcc -g -march=native -DX86CPU
SDL = `sdl-config --cflags`
#==Consoles==
#==GP2X/WIZ==
#ORIGINALFW = -static -lpng -lpthread -lz -ljpeg -lm -s
#CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X
#SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
#INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
#LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
#==Caanoo==
#CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO
#SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
#LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
#==Dingoo==
#CPP = /opt/mipsel-linux-uclibc/usr/bin/mipsel-linux-gcc -DDINGOO
#SDL = -I/opt/mipsel-linux-uclibc/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/mipsel-linux-uclibc/usr/include
#LIB = -L/opt/mipsel-linux-uclibc/usr/lib -Wl,-rpath=/opt/mipsel-linux-uclibc/usr/lib
#==Pandora==
#CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DARMCPU -DPANDORA
#SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
#INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
#LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib -lpnd

all: testengine

testengine: testengine.c meshloader.o 3dengine.o graphicstuff.o graphicstuff-asm.o
	$(CPP) -O3 testengine.c 3dengine.o graphicstuff.o meshloader.o graphicstuff-asm.o $(SDL) $(INCLUDE) $(LIB) -lSDL_image -lSDL -lm $(ORIGINALFW) -o testengine

meshloader.o: meshloader.c meshloader.h 3dengine.o
	$(CPP) -O3 -c meshloader.c $(SDL) $(INCLUDE)

3dengine.o: 3dengine.c 3dengine.h graphicstuff.o
	$(CPP) -O3 -c 3dengine.c -w $(SDL) $(INCLUDE)

graphicstuff.o: graphicstuff.c graphicstuff.h graphicstuff.o
	$(CPP) -O3 -c graphicstuff.c $(SDL) $(INCLUDE)

graphicstuff-asm.o: graphicstuff-asm.c graphicstuff.h
	$(CPP) -O2 -c graphicstuff-asm.c $(SDL) $(INCLUDE)

clean:
	rm *.o
	rm testengine	
