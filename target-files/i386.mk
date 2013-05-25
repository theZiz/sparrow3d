# === Target file for the i386 linux. Use it with "make TARGET=i386" ===
CPP = gcc -march=i386 -DX86CPU $(GENERAL_TWEAKS)
