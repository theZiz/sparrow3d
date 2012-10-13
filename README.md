Sparrow3D
=========

1. About Sparrow3D
2. Licence
3. Compiling Sparrow3D
4. Author

-------------------------------------------------------------------------------

1. About Sparrow3D
==================

Sparrow3D is an application framework with software renderer especially for
open handhelds. The goal is to just write an application once and to compile
it e.g. for:
- GP2X F100 / F200
- GP2X WIZ
- GP2X Caanoo
- Dingux
- Pandora
- Maemo (Future)
- PC (Linux / Windows) (mainly for testing...)
That are the systems, which are supported at the moment or which hopefully will
be supported in the near future. :)
To see, what is possible with this engine take a look at
- einstein (TODO: rewrite & link)
- snowman (TODO: rewrite & link)
- puzzletube (TODO: rewrite & link)

-------------------------------------------------------------------------------


2. Licence
==========

Not much to say: Just (L)GPL and MPL. If you want another Licence (e.g. some
kind of BSD licence) feel free to contact me. ;-)

-------------------------------------------------------------------------------

3. Compiling Sparrow3D
======================

First have a look in the Makefile. Some platforms are comment out. Just comment
out the first PC-Variables and comment in your favourate platform. Keep in mind
to change to location of the toolchain tools and include directories ;-)

If everything fine with your Makefile, just type
make
Thats all. Their is not possibility to install or such a thing ^^
Keep in mind, this engine and the games using this engines are mode for
(hopefully) open handhelds! Their is no need to install anything...
If you want to test the testsparrow-application on your handheld / PC use the
./testsparrow.sh file. If the file doesn't work on your system or you just
don't want to use it, keep sure, that the application findes the
libsparrow3d.whatever (e.g. so on linux system or dll on windows machines)

Here is a list of compile-switches used throughout sparrow (use with -D...):
BUILDING_DLL
	Windows only, changes dllimport to dllexport, if you are running
	into errors during linking, compile with this define
CORE_DEBUG
	Print out verbose debug text
FAST_BUT_UGLY 
	could crash, uses possibly memory (just reading) Try always to have some 
	border pixels to your textures if using!
FAST_MULTIPLICATION
	looks good at small resolutions
FAST_DIVISION
	looks good at small resolutions
SDL_INCLUDE_SUBDIR
	Changes the include path for every SDL*.h file to SDL/SDL*.h
   
Target platforms (defines, use with -D...):
X86CPU		Generic 32bit PC
WIN32		Windows operating system (usually set automatically)
ARMCPU		Generic mobile device, used in combination with a device switch
GP2X		Second and third generation of GPH devices, use with one of the
			following switches: F100, WIZ
CAANOO		GPH Caanoo
DINGUX		Dingoo with dingux Linux
GCW	GCW
PANDORA		Pandora

-------------------------------------------------------------------------------


4. Author
=========

Alexander Matthes aka Ziz
mail: zizsdl@googlemail.com
jabber: ziz@jabber.ccc.de
irc: nickname "Ziz" in:
- irc.euirc.net
- irc.efnet.ch
- irc.freenode.net

I think you read, that I am no Englisch native speaker. So ignore the bad
grammar or give me corrections. I always try to become better. :)
