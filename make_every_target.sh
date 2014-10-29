#!/bin/sh
FILES=./target-files/*
echo "Compiling for all targets..."
for f in $FILES
do
	TARGET=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
	make oclean > /dev/null
	make -j 4 TARGET=$TARGET PARAMETER=-w all> /dev/null
	if [ $? -ne 0 ]; then
		echo "Error compiling for \033[1;31m$TARGET\033[0m!"
	else
		echo "Everything fine with \033[1;32m$TARGET\033[0m!"
	fi
done
echo "Compiling for default..."
make clean > /dev/null
make > /dev/null
