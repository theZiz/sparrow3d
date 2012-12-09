# Hi and welcome to the "sparrow3d Makefile". This Makefile is for compiling
# sparrow3d on unix based systems or Windows with Mingw32/Cygwin (not tested)

# === First of all some defines. If your compilation is slow or instable, check
# === the defines and changes them for your Target.

# -DDO_USE_NOT_WASD_BUTTONS enables other buttons instead of WASD for the
# action buttons and QE for the shoulder buttons. For debugging and some
# direction based games QWEASD is fine, but for release, ctrl, alt and
# so one are better I think. More details in sparrowDefines.h. Of course
# this defines makes only sense for PC. ;) However, it is a good idea
# to use make OUTSIDE_FLAG=-DDO_USE_NOT_WASD_BUTTONS.

# -DFAST_MULTIPLICATION enables a faster multiplication for fixed point
# arithmetics, it may look ugly on bigger resolution or may be too inaccurate.

# -DFAST_DIVISION does the same as -DFAST_MULTIPLICATION, but for division.

# -DFAST_BUT_UGLY may crash. It uses possibly memory (just reading),
# which it not owns. Try always to have some border pixels to your
# textures if using! However as it says, it makes sparrow3d a bit faster,
# but the texture mapping a bit uglier.

# If you activate -DDO_NOT_USE_DELAY there will be no delay of 1 ms, if
# the time between two calculations is 0. If your device is powerfull
# don't use it. I activate it only for the gp2x.

# === Now some globale variables. These are the default values for compilation on
# === a Linux machine. The other targets then change, where their have differences

# DYNAMIC says, which libraries will be linked dynamicly. Most of the time these
# are all used libraries, but some systems also need static linking, too. Because
# as default no library is linked static, STATIC is not defined yet.
DYNAMIC =  -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm
# -lSDL_net

# CFLAGS defines some globale flags for gcc. Even on the gp2x with only 16 KB
# CPU Cache, -O3 is faster than -Os. So most things you don't have to change
CFLAGS = -O2 -fsingle-precision-constant -fPIC

# GENERAL_TWEAKS are some flags for gcc, which should make the compilation
# faster, but some of them are just poinsoness snake oil - they may help a bit,
# but could also kill you. ^^
GENERAL_TWEAKS = -ffast-math $(OUTSIDE_FLAG)

# every device using SMALL_RESOLUTION_DEVICES in the compilation will enable
# the faster multiplication and division
SMALL_RESOLUTION_DEVICES = -DFAST_MULTIPLICATION -DFAST_DIVISION

# The default Compiler is gcc with debug symbols and a X86CPU (that's a define
# for sparrow or your game, not used by gcc.
CPP = gcc -march=native -DX86CPU -g $(GENERAL_TWEAKS)

# SDL sets some SDL options with the program "sdl-config".
SDL = `sdl-config --cflags`

# SPARROW_LIB determines, where the sparrow library is.
SPARROW_LIB = -L.

# SPARROW_INCLUDE determines, where the sparrow includes are
SPARROW_INCLUDE = -I.

# SDL_PATH determines, where the SDL includes are (necessary if SDL is in subfolders)
SDL_PATH = -I/usr/include/SDL

# Uncomment one of this lines to create or use profile files for
# compiling. You can compile with -fprofile-generate for ANY target,
# copy the *.gcda files to the source code folder and recompile
# (complete!) with -fprofile-use again. This uses some in runtime found
# possibilities to improve sparrow3d.
# CFLAGS += -fprofile-generate
# CFLAGS += -fprofile-use

# INCLUDE determines, where all the other includes are
# INCLUDE = -I/usr/include

# LIB determines, where all the other libraries are
# LIB = -L/usr/lib

# === The Targets. Set your own paths on your PC!
# == GP2X/WIZ ==
ifeq ($(TARGET),open2x)
# CPP_LINK = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100 -DFAST_BUT_UGLY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
# CPP = /opt/open2x/wiz-toolchain-4.6.3-3/bin/arm-wiz-linux-gnu-gcc -mfpu=fpa -msoft-float -mcpu=arm920t -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100 -DFAST_BUT_UGLY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DREALGP2X -DF100 -DFAST_BUT_UGLY -DDO_NOT_USE_DELAY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags` 
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
ifeq ($(TARGET),gp2x)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DREALGP2X -DF100 -DFAST_BUT_UGLY -DDO_NOT_USE_DELAY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC = -Wl,-Bstatic -lSDL -lm -Wl,-Bdynamic
DYNAMIC = -lSDL_image -lSDL_net -lSDL_mixer -lSDL_ttf -lfreetype -lpng -lz -ljpeg
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
ifeq ($(TARGET),wiz)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DWIZ $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC = -Wl,-Bstatic -lpng -Wl,-Bdynamic
DYNAMIC = -lSDL_net -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm -lsparrow3d
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
# == Caanoo ==
ifeq ($(TARGET),caanoo)
CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
endif
# == Dingux ==
ifeq ($(TARGET),dingux)
CPP = /opt/opendingux-toolchain/usr/bin/mipsel-linux-gcc -DMOBILE_DEVICE -DDINGUX -DFAST_BUT_UGLY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = -I/opt/opendingux-toolchain/usr/mipsel-unknown-linux-uclibc/sys-include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/opendingux-toolchain/usr/mipsel-unknown-linux-uclibc/sys-include
LIB = -L/opt/opendingux-toolchain/usr/lib -Wl,-rpath=/opt/opendingux-toolchain/usr/lib
endif
# == GCW ==
ifeq ($(TARGET),gcw)
CPP = /opt/opendingux-toolchain/usr/bin/mipsel-linux-gcc -DMOBILE_DEVICE -DGCW -DFAST_BUT_UGLY $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = -I/opt/opendingux-toolchain/usr/mipsel-unknown-linux-uclibc/sys-include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/opendingux-toolchain/usr/mipsel-unknown-linux-uclibc/sys-include
LIB = -L/opt/opendingux-toolchain/usr/lib -Wl,-rpath=/opt/opendingux-toolchain/usr/lib
endif
# == Pandora ==
ifeq ($(TARGET),pandora)
CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DPANDORA $(GENERAL_TWEAKS)
SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib
endif

# I tried a bit with different compilers for building and linking. However: That just sets CPP_LINK to CPP. ;-)
CPP_LINK = $(CPP)

all: libsparrow3d.so libsparrowSound.so libsparrowNet.so testsparrow testsprite testmesh testtarget testtext testfile testparallax testserver testclient

targets:
	@echo "gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, gcw"

testsparrow: testsparrow.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testsparrow.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testsparrow

testsprite: testsprite.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testsprite.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testsprite

testmesh: testmesh.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testmesh.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testmesh

testtarget: testtarget.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testtarget.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testtarget

testtext: testtext.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testtext.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testtext

testfile: testfile.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testfile.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testfile

testparallax: testparallax.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testparallax.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o testparallax

testclient: testclient.c libsparrow3d.so libsparrowNet.so
	$(CPP_LINK) $(CFLAGS) testclient.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -lsparrowNet -o testclient

testserver: testserver.c libsparrow3d.so libsparrowNet.so
	$(CPP_LINK) $(CFLAGS) testserver.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -lsparrowNet -o testserver

libsparrow3d.so: sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowFont.o sparrowMesh.o sparrowSprite.o sparrowText.o sparrowFile.o
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrow3d.so -rdynamic -o libsparrow3d.so sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowMesh.o sparrowSprite.o sparrowFile.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowText.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

libsparrowSound.so: sparrowSound.o
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrowSound.so -rdynamic -o libsparrowSound.so sparrowSound.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

libsparrowNet.so: sparrowNet.o
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrowNet.so -rdynamic -o libsparrowNet.so sparrowNet.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

sparrowCore.o: sparrowCore.c sparrowCore.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowCore.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowPrimitives.o: sparrowPrimitives.c sparrowPrimitives.h sparrowPrimitiveSetPixelInclude.c sparrowPrimitiveTriangleInclude.c sparrowPrimitiveTexTriangleInclude.c
	$(CPP) $(CFLAGS) -fPIC -c sparrowPrimitives.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowPrimitivesAsm.o: sparrowPrimitivesAsm.c sparrowPrimitives.h
	$(CPP) $(CFLAGS) -fsingle-precision-constant -fPIC -c sparrowPrimitivesAsm.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowRenderer.o: sparrowRenderer.c sparrowRenderer.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowRenderer.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowMath.o: sparrowMath.c sparrowMath.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowMath.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowFont.o: sparrowFont.c sparrowFont.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowFont.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowMesh.o: sparrowMesh.c sparrowMesh.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowMesh.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowSprite.o: sparrowSprite.c sparrowSprite.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowSprite.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowText.o: sparrowText.c sparrowText.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowText.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowFile.o: sparrowFile.c sparrowFile.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowFile.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowSound.o: sparrowSound.c sparrowSound.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowSound.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowNet.o: sparrowNet.c sparrowNet.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowNet.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

clean:
	rm -f *.o
	rm -f libsparrow3d.so
	rm -f testsparrow
	rm -f testsprite
	rm -f testmesh
	rm -f testtext
	rm -f testtarget
	rm -f testparallax
	rm -f testclient
	rm -f testserver

# This Makefile is AWESOME! /)^ɛ^(\ 
