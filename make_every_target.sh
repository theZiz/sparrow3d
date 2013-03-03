#!/bin/sh
FILES=./target-files/*
for f in $FILES
do
  make clean
  make TARGET=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
done
make clean
make
