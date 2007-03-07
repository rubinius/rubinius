#!/bin/sh
if [ `uname` = 'Darwin' ] ; then
	echo '-dynamic'
else
	echo '-fPIC'
fi
