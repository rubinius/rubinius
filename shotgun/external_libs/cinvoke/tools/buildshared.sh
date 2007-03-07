#!/bin/sh
if [ `uname` = 'Darwin' ] ; then
	echo "libtool -dynamic -o $1 $2 -lc"
	libtool -dynamic -o $1 $2 -lc
else
	echo "gcc -shared -o $1 $2"
	gcc -shared -o $1 $2
fi
