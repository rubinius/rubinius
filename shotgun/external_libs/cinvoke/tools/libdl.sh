#!/bin/sh

if [ -f /lib/libdl.so.2 -o -f /usr/lib/libdl.dylib ]
then
	echo '-ldl'
fi

