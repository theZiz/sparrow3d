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

# -DDO_USE_NOT_WASD_BUTTONS enables other buttons instead of WASD for the
# action buttons and QE for the shoulder buttons. For debugging and some
# direction based games QWEASD is fine, but for release, ctrl, alt and
# so one are better I think. More details in sparrowDefines.h. Of course
# this defines makes only sense for PC. ;) However, it is a good idea
# to use make OUTSIDE_FLAG=-DDO_USE_NOT_WASD_BUTTONS.

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

# This deactivates the perspective mapping and makes the lib smaller,
# but as I tested not faster.
# CFLAGS += -DNO_PERSPECTIVE

# Normaly theese flags are activated only for small and/or slow devices.
# But sometimes it is important to activate them for testing purpose
# even on faster hardware. ;)
# CFLAGS += -DFAST_DIVISION -DFAST_MULTIPLICATION

# Debug flag for slow motion in the whole engine! Including drawing stuff. Set
# the speed divisor you want to use (5 is default if activated, 1 does nothing
# obviously. Even spSleep is slow moooooo'ed.
# CFLAGS += -DDEBUG_SLOWMOTION=5

ifdef TARGET
include ./target-files/$(TARGET).mk
BUILD = ./build/$(TARGET)/sparrow3d
LIB += -L$(BUILD)
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
endif

CPP += $(PARAMETER)

# I tried a bit with different compilers for building and linking. However: That just sets CPP_LINK to CPP. ;-)
CPP_LINK = $(CPP)

all: libsparrow3d.so libsparrowSound.so libsparrowNet.so testsparrow testsprite testtarget testtext testfile testparallax testserver testclient testreal3d testfill
	@echo "=== Built for Target "$(TARGET)" ==="

targets:
	@echo "The targets are:"
	@ls ./target-files/*.mk | cut -d/ -f3 | cut -d. -f1
#	@echo "Default targets are: gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, gcw"

testsparrow: testsparrow.c
	$(CPP_LINK) $(CFLAGS) testsparrow.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testsparrow

testreal3d: testreal3d.c
	$(CPP_LINK) $(CFLAGS) testreal3d.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testreal3d

testsprite: testsprite.c
	$(CPP_LINK) $(CFLAGS) testsprite.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testsprite

testtarget: testtarget.c
	$(CPP_LINK) $(CFLAGS) testtarget.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testtarget

testtext: testtext.c
	$(CPP_LINK) $(CFLAGS) testtext.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testtext

testfile: testfile.c
	$(CPP_LINK) $(CFLAGS) testfile.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testfile

testfill: testfill.c
	$(CPP_LINK) $(CFLAGS) testfill.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testfill

testparallax: testparallax.c
	$(CPP_LINK) $(CFLAGS) testparallax.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/testparallax

testclient: testclient.c
	$(CPP_LINK) $(CFLAGS) testclient.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -lsparrowNet -o $(BUILD)/testclient

testserver: testserver.c
	$(CPP_LINK) $(CFLAGS) testserver.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -lsparrowNet -o $(BUILD)/testserver

libsparrow3d.so: sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowFont.o sparrowMesh.o sparrowSprite.o sparrowText.o sparrowFile.o sparrowGUI.o makeBuildDir
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrow3d.so -rdynamic -o $(BUILD)/libsparrow3d.so sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowMesh.o sparrowSprite.o sparrowFile.o sparrowGUI.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowText.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

libsparrowSound.so: sparrowSound.o makeBuildDir
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrowSound.so -rdynamic -o $(BUILD)/libsparrowSound.so sparrowSound.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

libsparrowNet.so: sparrowNet.o makeBuildDir
	$(CPP_LINK) $(CFLAGS) -shared -Wl,-soname,libsparrowNet.so -rdynamic -o $(BUILD)/libsparrowNet.so sparrowNet.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

makeBuildDir:
	 @if [ ! -d $(BUILD:/sparrow3d=/) ]; then mkdir $(BUILD:/sparrow3d=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

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

sparrowSound.o: sparrowSound.c sparrowSound.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowSound.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

sparrowNet.o: sparrowNet.c sparrowNet.h
	$(CPP) $(CFLAGS) -fPIC -c sparrowNet.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

clean:
	rm -f *.o
	rm -f libsparrow3d.so
	rm -f testsparrow
	rm -f testsprite
	rm -f testtext
	rm -f testtarget
	rm -f testparallax
	rm -f testclient
	rm -f testserver

documentation: *.h
	mkdir -p /tmp/sparrowDocumentationTemp
	naturaldocs -i . -xi ./documentation -o HTML documentation -p /tmp/sparrowDocumentationTemp

# This Makefile is AWESOME! /)^ɛ^(\ 
