Sparrow3D
=========

1. About Sparrow3D
2. Licence
3. Documentation
4. Compiling Sparrow3D
5. Author

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
- GCW Zero
- PC (Linux / Windows) (mainly for testing...)

That are the systems, which are supported at the moment or which hopefully will
be supported in the near future. :)
To see, what is possible with this engine take a look at
- Puzzletube
- Snowman
- Schwerkaft

-------------------------------------------------------------------------------

2. Licence
==========

Not much to say: Just (L)GPL. If you want another Licence (e.g. some
kind of BSD licence) feel free to contact me. ;-)

-------------------------------------------------------------------------------

3. Documentation
==========

You can find the documentation [here][docu] and furthermore some first
steps and examples in the [github wiki][wiki].
[docu]: http://ziz.gp2x.de/documentation
[wiki]: https://github.com/theZiz/sparrow3d/wiki

-------------------------------------------------------------------------------

4. Compiling Sparrow3D
======================

For compiling for PC just type  
make  
Thats all. Their is no possibility to install or such a thing ^^  
For handhelds type  
make targets  
to get the targets and  
make TARGET=target  
to compile for the target "target". Every target has a .mk file in the folder
targets. Change the paths in these files to your paths.
Keep in mind, this engine and the games using this engines are mode for
(hopefully) open handhelds! Their is no need to install anything...
If you want to test the testsparrow-application on your handheld / PC use the
./testsparrow.sh file. If the file doesn't work on your system or you just
don't want to use it, keep sure, that the application finds the
libsparrow3d.whatever (e.g. .so on linux system or .dll on windows machines)

Here is a list of compile-switches used throughout sparrow (use with -D...):

BUILDING_DLL  
-	Windows only, changes dllimport to dllexport, if you are running
	into errors during linking, compile with this define

CORE_DEBUG  
-	Print out verbose debug text

UNSAFE_MAGIC  
-	could crash, uses possibly memory (just reading) Try always to have some 
	border pixels to your textures if using!

BLACK_MAGIC  
-	Uses fancy lookup table magic to improve speed, but may look worse. Just try.

FAST_MULTIPLICATION  
-	looks good at small resolutions

FAST_DIVISION  
-	looks good at small resolutions

SDL_INCLUDE_SUBDIR  
-	Changes the include path for every SDL*.h file to SDL/SDL*.h
   
Target platforms (defines, use with -D...):

GP2X  
-	Second and third generation of GPH devices, use with one of the following
	switches: F100, WIZ

CAANOO 
-	GPH Caanoo

DINGUX  
-	Dingoo with dingux Linux

GCW  
-	GCW Zero

PANDORA 
-	Pandora

-------------------------------------------------------------------------------


5. Author
=========

Alexander Matthes aka Ziz
- mail: zizsdl@googlemail.com
- jabber: ziz@jabber.ccc.de
- irc: nickname "Ziz" in:
    - irc.euirc.net
    - irc.efnet.ch
    - irc.freenode.net
- site: ziz.openhandhelds.org

I think you read, that I am not an Englisch native speaker. So ignore the bad
grammar or give me corrections. I always try to get better. :)
