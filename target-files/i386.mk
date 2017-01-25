# === Target file for the i386 linux. Use it with "make TARGET=i386" ===

CC = gcc
FLAGS = -m32 $(GENERAL_TWEAKS) -DDESKTOP
LIB = -L/usr/lib/i386-linux-gnu
DYNAMIC =  -lSDL_mixer -lSDL_ttf -lSDL_image -lSDL -lm -lSDL_net
