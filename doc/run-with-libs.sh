#!/bin/sh

REAL=shotgun/rubinius.bin

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:shotgun/lib
export LD_LIBRARY_PATH_64=$LD_LIBRARY_PATH_64:shotgun/lib
export SH_LIB=$SH_LIB:shotgun/lib
export LIBPATH=$LIBPATH:shotgun/lib
export PATH=$PATH:shotgun/lib
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:shotgun/lib

exec $REAL $@
