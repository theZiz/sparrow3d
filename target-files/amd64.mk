# === Target file for the amd64 linux. Use it with "make TARGET=amd64" ===
CPP = gcc -march=x86-64 -DX86CPU $(GENERAL_TWEAKS)
