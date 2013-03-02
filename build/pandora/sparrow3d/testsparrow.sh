#!/bin/sh
export PATH=":${PATH:-"/usr/bin:/bin:/usr/local/bin"}"
export LD_LIBRARY_PATH=":${LD_LIBRARY_PATH:-"/usr/lib:/lib"}"
export HOME="/mnt/utmp/testsparrow" XDG_CONFIG_HOME="/mnt/utmp/testsparrow"

if [ -d /mnt/utmp/testsparrow/share ];then
	export XDG_DATA_DIRS=/mnt/utmp/testsparrow/share:$XDG_DATA_DIRS:/usr/share
fi
export SDL_AUDIODRIVER="alsa"

cat /proc/pandora/nub0/mode > /tmp/nub0mode_before_testsparrow
cat /proc/pandora/nub1/mode > /tmp/nub1mode_before_testsparrow
echo absolute > /proc/pandora/nub0/mode
echo absolute > /proc/pandora/nub1/mode

LD_LIBRARY_PATH=. ./testsparrow $*

cat /tmp/nub0mode_before_testsparrow > /proc/pandora/nub0/mode
cat /tmp/nub1mode_before_testsparrow > /proc/pandora/nub1/mode
 
rm /tmp/nub0mode_before_testsparrow /tmp/nub1mode_before_testsparrow
