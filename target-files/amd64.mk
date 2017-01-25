# === Target file for the amd64 linux. Use it with "make TARGET=amd64" ===

CC = gcc
FLAGS =  -march=x86-64 $(GENERAL_TWEAKS) -DDESKTOP
