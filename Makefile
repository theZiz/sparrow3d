# Hi and welcome to the "sparrow3d Makefile". This Makefile is for compiling
# sparrow3d on unix based systems or Windows with Mingw32/Cygwin

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
CFLAGS = -O3 -fsingle-precision-constant -fPIC

# GENERAL_TWEAKS are some flags for gcc, which should make the compilation
# faster, but some of them are just poinsoness snake oil - they may help a bit,
# but could also kill you. ^^
GENERAL_TWEAKS = -ffast-math $(OUTSIDE_FLAG)

# every device using SMALL_RESOLUTION_DEVICES in the compilation will enable
# the faster multiplication and division
SMALL_RESOLUTION_DEVICES = -DFAST_MULTIPLICATION -DFAST_DIVISION

# The default Compiler is CC (gcc) with debug symbols

FLAGS = -g $(GENERAL_TWEAKS) -DDESKTOP

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

SPARROW_TEST = testsparrow testfile testnet testreal3d testnet_terminal_server testnet_terminal_client testsimple

SPARROW3D_LIB = libsparrow3d.so
SPARROWNET_LIB = libsparrowNet.so
SPARROWSOUND_LIB = libsparrowSound.so
SPARROW3D_STATIC_LIB = libsparrow3d.a
SPARROWNET_STATIC_LIB = libsparrowNet.a
SPARROWSOUND_STATIC_LIB = libsparrowSound.a
RDYNAMIC = -rdynamic

ifdef TARGET
include ./target-files/$(TARGET).mk

BUILD = ./build/$(TARGET)/sparrow3d
# SPARROW_LIB determines, where the sparrow library is.
SPARROW_LIB += -L$(BUILD)
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
# SPARROW_LIB determines, where the sparrow library is.
SPARROW_LIB += -L.
endif

CFLAGS += $(PARAMETER) $(FLAGS) -Wno-pointer-to-int-cast

ifndef AR
AR = ar
endif

ifndef RANLIB
RANLIB = ranlib
endif

SPARROW_LIBS = $(SPARROW3D_LIB) $(SPARROWNET_LIB) $(SPARROWSOUND_LIB)
SPARROW_STATIC_LIBS = $(SPARROW3D_STATIC_LIB) $(SPARROWNET_STATIC_LIB) $(SPARROWSOUND_STATIC_LIB)

all: static all_no_static

all_no_static: dynamic $(SPARROW_TEST)
	@echo "=== Built for Target "$(TARGET)" ==="

static: $(SPARROW_STATIC_LIBS)

dynamic: $(SPARROW_LIBS)

targets:
	@echo "The targets are:"
	@ls ./target-files/*.mk | cut -d/ -f3 | cut -d. -f1
#	@echo "Default targets are: gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, gcw"

testsparrow: testsparrow.c $(SPARROW3D_LIB) test_cube.o test_fill.o test_gears.o test_mesh.o test_primitives.o test_sprites.o test_tube.o test_yinyang.o test_text.o test_target.o test_mapping.o
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< test_cube.o test_fill.o test_gears.o test_mesh.o test_primitives.o test_sprites.o test_tube.o test_yinyang.o test_text.o test_target.o test_mapping.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/$@

testsimple: testsimple.c $(SPARROW3D_LIB)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/$@

testreal3d: testreal3d.c $(SPARROW3D_LIB)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/$@

testfile: testfile.c $(SPARROW3D_LIB)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -o $(BUILD)/$@

testnet: testnet.c $(SPARROW3D_LIB) $(SPARROWNET_LIB)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -lsparrow3d -lsparrowNet -o $(BUILD)/$@

testnet_terminal_server: testnet_terminal_server.c $(SPARROWNET_LIB)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) -lSDL_net -lsparrowNet -o $(BUILD)/$@

testnet_terminal_client: testnet_terminal_client.c $(SPARROWNET_LIB)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SPARROW_LIB) -lSDL_net -lsparrowNet -o $(BUILD)/$@

makeBuildDir:
	@if [ ! -d $(BUILD:/sparrow3d=/) ]; then mkdir $(BUILD:/sparrow3d=/);fi
	@if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

$(SPARROW3D_STATIC_LIB): sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowFont.o sparrowMesh.o sparrowSprite.o sparrowText.o sparrowFile.o sparrowMapping.o sparrowParticles.o makeBuildDir
	$(AR) rcs $(BUILD)/$(SPARROW3D_STATIC_LIB) sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowMesh.o sparrowSprite.o sparrowFile.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowText.o sparrowMapping.o sparrowParticles.o
	$(RANLIB) $(BUILD)/$(SPARROW3D_STATIC_LIB)

$(SPARROWSOUND_STATIC_LIB): sparrowSound.o makeBuildDir
	$(AR) rcs $(BUILD)/$(SPARROWSOUND_STATIC_LIB) sparrowSound.o
	$(RANLIB) $(BUILD)/$(SPARROWSOUND_STATIC_LIB)

$(SPARROWNET_STATIC_LIB): sparrowNet.o makeBuildDir
	$(AR) rcs $(BUILD)/$(SPARROWNET_STATIC_LIB) sparrowNet.o
	$(RANLIB) $(BUILD)/$(SPARROWNET_STATIC_LIB)

$(SPARROW3D_LIB): sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowFont.o sparrowMesh.o sparrowSprite.o sparrowText.o sparrowFile.o sparrowMapping.o sparrowParticles.o makeBuildDir
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(SPARROW3D_LIB) $(RDYNAMIC) -o $(BUILD)/$(SPARROW3D_LIB) sparrowFont.o sparrowCore.o sparrowMath.o sparrowPrimitives.o sparrowMesh.o sparrowSprite.o sparrowFile.o sparrowPrimitivesAsm.o sparrowRenderer.o sparrowText.o sparrowMapping.o sparrowParticles.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

$(SPARROWSOUND_LIB): sparrowSound.o makeBuildDir
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(SPARROWSOUND_LIB) $(RDYNAMIC) -o $(BUILD)/$(SPARROWSOUND_LIB) sparrowSound.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

$(SPARROWNET_LIB): sparrowNet.o makeBuildDir
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(SPARROWNET_LIB) $(RDYNAMIC) -o $(BUILD)/$(SPARROWNET_LIB) sparrowNet.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(STATIC) $(DYNAMIC)

%.o: %.c %.h Makefile
	$(CC) $(CFLAGS) -fPIC -c $< $(SDL) $(INCLUDE) $(SDL_INCLUDE) -DBUILDING_DLL $(SPARROW_INCLUDE)

clean:
	rm -f *.o
	cd $(BUILD); rm -f $(SPARROW3D_LIB) $(SPARROWNET_LIB) $(SPARROWSOUND_LIB)
	cd $(BUILD); rm -f $(SPARROW3D_STATIC_LIB) $(SPARROWNET_STATIC_LIB) $(SPARROWSOUND_STATIC_LIB)
	cd $(BUILD); rm -f $(SPARROW_TEST)

oclean:
	rm -f *.o

documentation: *.h
	mkdir -p /tmp/sparrowDocumentationTemp
	naturaldocs -i . -xi ./documentation -o HTML documentation -p /tmp/sparrowDocumentationTemp

# This Makefile is AWESOME! /)^ɛ^(\ 
