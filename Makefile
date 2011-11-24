#==Use if to slow!==
# -DFAST_BUT_UGLY <- could crash, uses possibly memory (just reading) Try always to have some border pixels to your textures if using!
# -DFAST_BUT_UGLY_2
#==global Flags. Even on the gp2x with 16 kb Cache, -O3 is much better then -Os
CFLAGS = -O3 -fsingle-precision-constant -fPIC 
CFLAGS_ASM = -O2 -fsingle-precision-constant -fPIC
GENERAL_TWEAKS = -DFAST_BUT_UGLY -DFAST_BUT_UGLY_2 -ffast-math -DCORE_DEBUG
#==PC==
CPP = gcc -g -march=native -DX86CPU $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`
#==Consoles==
#==GP2X/WIZ==
#ORIGINALFW = -static -lfreetype -lpng -lpthread -lz -ljpeg -lm -s
ifeq ($(TARGET),gp2x)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X $(GENERAL_TWEAKS) -DFAST_BUT_UGLY_2 
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
#==Caanoo==
ifeq ($(TARGET),caanoo)
CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO $(GENERAL_TWEAKS)
SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
endif
#==Dingoo==
ifeq ($(TARGET),dingoo)
CPP = /opt/mipsel-linux-uclibc/usr/bin/mipsel-linux-gcc -DDINGOO $(GENERAL_TWEAKS)
SDL = -I/opt/mipsel-linux-uclibc/usr/mipsel-linux-uclibc/sys-include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/mipsel-linux-uclibc/usr/mipsel-linux-uclibc/sys-include
LIB = -L/opt/mipsel-linux-uclibc/usr/lib -Wl,-rpath=/opt/mipsel-linux-uclibc/usr/lib
endif
#==Pandora==
ifeq ($(TARGET),pandora)
CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DARMCPU -DPANDORA $(GENERAL_TWEAKS)
SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
#-lpnd
LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib
endif
#==Maemo 5==
ifeq ($(TARGET),maemo5)
CPP = gcc -DARMCPU -DMAEMO -DMAEMO5 $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`
endif
#==Maemo 6==
ifeq ($(TARGET),maemo6)
CPP = gcc -DARMCPU -DMAEMO -DMAEMO6 $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`
endif


all: sparrow3d testsparrow

targets:
	@echo "gp2x (=wiz), caanoo, dingoo, pandora, maemo5, maemo6"

testsparrow: testsparrow.c sparrow3d
	$(CPP) $(CFLAGS) testsparrow.c $(SDL) $(INCLUDE) -L. $(LIB) -lSDL_ttf -lSDL_image -lSDL -lm -lsparrow3d $(ORIGINALFW) -o testsparrow

sparrow3d: sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowFont.o
	$(CPP) $(CFLAGS) -shared -Wl,-soname,libsparrow3d.so -o libsparrow3d.so sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o $(SDL) $(INCLUDE) $(LIB) -lSDL_ttf -lSDL_image -lSDL -lm $(ORIGINALFW)

sparrowCore.o: sparrowCore.c sparrowCore.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowCore.c $(SDL) $(INCLUDE)

sparrowPrimitives.o: sparrowPrimitives.c sparrowPrimitives.h
	$(CPP) $(CFLAGS)  -c sparrowPrimitives.c $(SDL) $(INCLUDE)

sparrowPrimitivesAsm.o: sparrowPrimitivesAsm.c sparrowPrimitives.h
	$(CPP) $(CFLAGS)  -fsingle-precision-constant -fPIC -c sparrowPrimitivesAsm.c $(SDL) $(INCLUDE)

sparrowRenderer.o: sparrowRenderer.c sparrowRenderer.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowRenderer.c $(SDL) $(INCLUDE)

sparrowMath.o: sparrowMath.c sparrowMath.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowMath.c $(SDL) $(INCLUDE)

sparrowFont.o: sparrowFont.c sparrowFont.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowFont.c $(SDL) $(INCLUDE)

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
