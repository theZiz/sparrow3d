# === Target file for the i386 linux. Use it with "make TARGET=i386" ===
CC = gcc
FLAGS = -march=i386 $(GENERAL_TWEAKS) -DDESKTOP
