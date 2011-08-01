#-mfloat-abi=softfp
CFLAGS = -O3
CFLAGS_ASM = -O2
#==PC==
#CXX = g++ -g -march=native -DX86CPU
CPP = gcc -g -march=native -DX86CPU
SDL = `sdl-config --cflags`
#==Consoles==
#==GP2X/WIZ==
#ORIGINALFW = -static -lfreetype -lpng -lpthread -lz -ljpeg -lm -s
#CXX = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-g++ -DMOBILE_DEVICE -DARMCPU -DGP2X
#CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X
#SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
#INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
#LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
#==Caanoo==
#CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO
#CXX = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-g++ -DMOBILE_DEVICE -DARMCPU -DCAANOO
#SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
#LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
#==Dingoo==
#CPP = /opt/mipsel-linux-uclibc/usr/bin/mipsel-linux-gcc -DDINGOO
#CXX = /opt/mipsel-linux-uclibc/usr/bin/mipsel-linux-g++ -DDINGOO
#SDL = -I/opt/mipsel-linux-uclibc/usr/mipsel-linux-uclibc/sys-include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
#INCLUDE = -I/opt/mipsel-linux-uclibc/usr/mipsel-linux-uclibc/sys-include
#LIB = -L/opt/mipsel-linux-uclibc/usr/lib -Wl,-rpath=/opt/mipsel-linux-uclibc/usr/lib
#==Pandora==
#CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DARMCPU -DPANDORA
#CXX = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-g++ -DARMCPU -DPANDORA
#SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
#INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
#LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib -lpnd

all: sparrow3d testsparrow

testsparrow: testsparrow.c sparrow3d
	$(CPP) $(CFLAGS) testsparrow.c $(SDL) $(INCLUDE) -L. $(LIB) -lSDL_ttf -lSDL_image -lSDL -lm -lsparrow3d $(ORIGINALFW) -o testsparrow

sparrow3d: sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowRenderer.o
	$(CPP) -shared -Wl,-soname,libsparrow3d.so -o libsparrow3d.so sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowRenderer.o $(SDL) $(INCLUDE) $(LIB) -lSDL_ttf -lSDL_image -lSDL -lm $(ORIGINALFW)

sparrowCore.o: sparrowCore.c sparrowCore.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowCore.c $(SDL) $(INCLUDE)

sparrowPrimitives.o: sparrowPrimitives.c sparrowPrimitives.h
	$(CPP) $(CFLAGS)  -fsingle-precision-constant -fPIC -c sparrowPrimitives.c $(SDL) $(INCLUDE)

sparrowRenderer.o: sparrowRenderer.c sparrowRenderer.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowRenderer.c $(SDL) $(INCLUDE)

sparrowMath.o: sparrowMath.c sparrowMath.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowMath.c $(SDL) $(INCLUDE)

clean:
	rm *.o
	rm libsparrow3d.so
	rm testsparrow

testengine: testengine.c meshloader.o 3dengine.o graphicstuff.o graphicstuff-asm.o
	$(CPP) $(CFLAGS) testengine.c 3dengine.o graphicstuff.o meshloader.o graphicstuff-asm.o $(SDL) $(INCLUDE) $(LIB) -lSDL_ttf -lSDL_image -lSDL -lm $(ORIGINALFW) -o testengine

meshloader.o: meshloader.c meshloader.h 3dengine.o
	$(CPP) $(CFLAGS) -c meshloader.c $(SDL) $(INCLUDE)

3dengine.o: 3dengine.c 3dengine.h graphicstuff.o
	$(CPP) $(CFLAGS) -c 3dengine.c -w $(SDL) $(INCLUDE)

graphicstuff.o: graphicstuff.c graphicstuff.h
	$(CPP) $(CFLAGS) -c graphicstuff.c $(SDL) $(INCLUDE)

graphicstuff-asm.o: graphicstuff-asm.c graphicstuff.h
	$(CPP) $(CFLAGS_ASM) -c graphicstuff-asm.c $(SDL) $(INCLUDE)
