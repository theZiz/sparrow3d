# Hi and welcome to the "sparrow3d Makefile". This Makefile is for compiling
# sparrow3d on unix based systems or Windows with Mingw32/Cygwin (not tested)

# === First of all some defines. If your compilation is slow or instable, check
# === the defines and changes them for your Target.
# === Every target is in an extra file named like the target + .mk, e.g.
# === gp2x.mk. If you have another path settings than me, just duplicate the
# === target file, alter the paths and call maybe make myGp2x if your file is
# === named myGp2x.mk. So you are independent from my changes of the target
# === files. ;) However always have a look at the changelog, maybe I add new
# === fancy optimization, you have to add in your target file yourself!

# -DFAST_MULTIPLICATION enables a faster multiplication for fixed point
# arithmetics, it may look ugly on bigger resolution or may be too inaccurate.

# -DFAST_DIVISION does the same as -DFAST_MULTIPLICATION, but for division.

# -DUNSAFE_MAGIC may crash. It uses possibly memory (just reading), which it
# not owns. Try always to have some border pixels to your textures if using!
# However it makes sparrow3d a bit faster, but the texture mapping a bit uglier.

# -DBLACK_MAGIC activates some crazy lookup table and other optimizations, but
# may look a bit worse.

# If you activate -DDO_NOT_USE_DELAY there will be no delay of 1 ms, if
# the time between two calculations is 0. If your device is powerfull
# don't use it. I activate it only for the gp2x.

# === Now some globale variables. These are the default values for compilation on
# === a Linux machine. The other targets then change, where their have
# === differences in the target files (the .mk files)

# DYNAMIC says, which libraries will be linked dynamicly. Most of the time these
# are all used libraries, but some systems also need static linking, too. Because
# as default no library is linked static, STATIC is not defined yet.
DYNAMIC =  -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm -lSDL_net

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
CPP = gcc -DX86CPU -g $(GENERAL_TWEAKS)

# SDL sets some SDL options with the program "sdl-config".
SDL = `sdl-config --cflags`

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

# This deactivates some features and makes the lib smaller, but as I
# tested not faster (of course you have differences between drawing with
# or without blending. That is just meant of speed increasement because
# of library size ;) )
# CFLAGS += -DNO_PERSPECTIVE
# CFLAGS += -DNO_BLENDING
# CFLAGS += -DNO_PATTERN

# Normaly theese flags are activated only for small and/or slow devices.
# But sometimes it is important to activate them for testing purpose
# even on faster hardware. ;)
# CFLAGS += -DFAST_DIVISION -DFAST_MULTIPLICATION

# Debug flag for slow motion in the whole engine! Including drawing stuff. Set
# the speed divisor you want to use (5 is default if activated, 1 does nothing
# obviously. Even spSleep is slow moooooo'ed.
# CFLAGS += -DDEBUG_SLOWMOTION=5

#TARGET = nativ

ifdef TARGET
include ./target-files/$(TARGET).mk

#TARGET = pandora

BUILD = ./build/$(TARGET)/sparrow3d
# SPARROW_LIB determines, where the sparrow library is.
SPARROW_LIB += -L$(BUILD)
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
# SPARROW_LIB determines, where the sparrow library is.
SPARROW_LIB += -L.
endif

CPP += $(PARAMETER)

# I tried a bit with different compilers for building and linking. However: That just sets CPP_LINK to CPP. ;-)
CPP_LINK = $(CPP)

all: dynamic static testsparrow testfile testnet testreal3d testnet_terminal_server testnet_terminal_client
	@echo "=== Built for Target "$(TARGET)" ==="

static: libsparrow3d.a libsparrowNet.a libsparrowSound.a

dynamic: libsparrow3d.so libsparrowNet.so libsparrowSound.so

targets:
	@echo "The targets are:"
	@ls ./target-files/*.mk | cut -d/ -f3 | cut -d. -f1
#	@echo "Default targets are: gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, gcw"

testsparrow: testsparrow.c libsparrow3d.so test_cube.o test_fill.o test_gears.o test_mesh.o test_primitives.o test_sprites.o test_tube.o test_yinyang.o test_text.o test_target.o test_mapping.o
	$(CPP_LINK) $(CFLAGS) testsparrow.c test_cube.o test_fill.o test_gears.o test_mesh.o test_primitives.o test_sprites.o test_tube.o test_yinyang.o test_text.o test_target.o test_mapping.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testsparrow

test_cube.o: test_cube.c test_cube.h
	$(CPP) $(CFLAGS) -fPIC -c test_cube.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_gears.o: test_gears.c test_gears.h
	$(CPP) $(CFLAGS) -fPIC -c test_gears.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_primitives.o: test_primitives.c test_primitives.h
	$(CPP) $(CFLAGS) -fPIC -c test_primitives.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_tube.o: test_tube.c test_tube.h
	$(CPP) $(CFLAGS) -fPIC -c test_tube.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_fill.o: test_fill.c test_fill.h
	$(CPP) $(CFLAGS) -fPIC -c test_fill.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_mesh.o: test_mesh.c test_mesh.h
	$(CPP) $(CFLAGS) -fPIC -c test_mesh.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_sprites.o: test_sprites.c test_sprites.h
	$(CPP) $(CFLAGS) -fPIC -c test_sprites.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_yinyang.o: test_yinyang.c test_yinyang.h
	$(CPP) $(CFLAGS) -fPIC -c test_yinyang.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_text.o: test_text.c test_text.h
	$(CPP) $(CFLAGS) -fPIC -c test_text.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_target.o: test_target.c test_target.h
	$(CPP) $(CFLAGS) -fPIC -c test_target.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

test_mapping.o: test_mapping.c test_mapping.h
	$(CPP) $(CFLAGS) -fPIC -c test_mapping.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

testreal3d: testreal3d.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testreal3d.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testreal3d

testfile: testfile.c libsparrow3d.so
	$(CPP_LINK) $(CFLAGS) testfile.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testfile

testnet: testnet.c libsparrow3d.so libsparrowNet.so
	$(CPP_LINK) $(CFLAGS) testnet.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -lsparrowNet -o $(BUILD)/testnet

testnet_terminal_server: testnet_terminal_server.c libsparrowNet.so
	$(CPP_LINK) $(CFLAGS) testnet_terminal_server.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) -lSDL_net -lsparrowNet -o $(BUILD)/testnet_terminal_server

testnet_terminal_client: testnet_terminal_client.c libsparrowNet.so
	$(CPP_LINK) $(CFLAGS) testnet_terminal_client.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) -lSDL_net -lsparrowNet -o $(BUILD)/testnet_terminal_client

libsparrow3d.a: libsparrow3d.so
	ar rcs $(BUILD)/libsparrow3d.a sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowMesh.o sparrowSprite.o sparrowFile.o sparrowGUI.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowText.o sparrowMapping.o

libsparrowSound.a: libsparrowSound.so
	ar rcs $(BUILD)/libsparrowSound.a sparrowSound.o

libsparrowNet.a: libsparrowNet.so
	ar rcs $(BUILD)/libsparrowNet.a sparrowNet.o

libsparrow3d.so: sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowFont.o sparrowMesh.o sparrowSprite.o sparrowText.o sparrowFile.o sparrowGUI.o sparrowMapping.o
	@if [ ! -d $(BUILD:/sparrow3d=/) ]; then mkdir $(BUILD:/sparrow3d=/);fi
	@if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrow3d.so -rdynamic -o $(BUILD)/libsparrow3d.so sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowMesh.o sparrowSprite.o sparrowFile.o sparrowGUI.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowText.o sparrowMapping.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

libsparrowSound.so: sparrowSound.o
	@if [ ! -d $(BUILD:/sparrow3d=/) ]; then mkdir $(BUILD:/sparrow3d=/);fi
	@if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrowSound.so -rdynamic -o $(BUILD)/libsparrowSound.so sparrowSound.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

libsparrowNet.so: sparrowNet.o
	@if [ ! -d $(BUILD:/sparrow3d=/) ]; then mkdir $(BUILD:/sparrow3d=/);fi
	@if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrowNet.so -rdynamic -o $(BUILD)/libsparrowNet.so sparrowNet.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

sparrowCore.o: sparrowCore.c sparrowCore.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowCore.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowPrimitives.o: sparrowPrimitives.c sparrowPrimitives.h sparrowPrimitiveSetPixelInclude.c sparrowPrimitiveTriangleInclude.c sparrowPrimitiveTexTriangleInclude.c sparrowPrimitiveDrawingThread.c sparrowPrimitiveDrawingThread.h sparrowPrimitiveHelperBlending.c sparrowPrimitiveHelperPattern.c sparrowPrimitiveHelperPerspective.c sparrowPrimitiveHelperZStuff.c
	$(CPP) $(CFLAGS) -fPIC -c sparrowPrimitives.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowPrimitivesAsm.o: sparrowPrimitivesAsm.c sparrowPrimitives.h
	$(CPP) $(CFLAGS) -fsingle-precision-constant -fPIC -c sparrowPrimitivesAsm.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

#sparrowPrimitiveDrawingThread.o: sparrowPrimitiveDrawingThread.c sparrowPrimitiveDrawingThread.h sparrowPrimitiveSetPixelInclude.c sparrowPrimitiveTriangleInclude.c sparrowPrimitiveTexTriangleInclude.c sparrowPrimitiveHelperBlending.c sparrowPrimitiveHelperPattern.c sparrowPrimitiveHelperPerspective.c sparrowPrimitiveHelperZStuff.c
#	$(CPP) $(CFLAGS) -fPIC -c sparrowPrimitiveDrawingThread.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

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

sparrowGUI.o: sparrowGUI.c sparrowGUI.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowGUI.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowMapping.o: sparrowMapping.c sparrowMapping.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowMapping.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowSound.o: sparrowSound.c sparrowSound.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowSound.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowNet.o: sparrowNet.c sparrowNet.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowNet.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f testsparrow
	rm -f testnet
	rm -f testfile

documentation: *.h
	mkdir -p /tmp/sparrowDocumentationTemp
	naturaldocs -i . -xi ./documentation -o HTML documentation -p /tmp/sparrowDocumentationTemp

# This Makefile is AWESOME! /)^ɛ^(\ 
