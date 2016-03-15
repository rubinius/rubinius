#!/bin/sh

export VERBOSE=1

if test "x$PROFILE" != "x"; then
  FLAGS="-fprofile-arcs -ftest-coverage"
else
  FLAGS=""
fi

vm/test/cxxtest/cxxtestgen.pl --error-printer --have-eh --abort-on-fail --include=string.h -o machine/test/one.cpp "$*"

g++ $FLAGS -g -Wall -Werror -Ivendor/libtommath -Imachine/test/cxxtest -I. -Ivm -o machine/test/one machine/test/one.cpp && ./machine/test/one
