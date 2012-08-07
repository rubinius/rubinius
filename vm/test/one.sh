#!/bin/sh

export VERBOSE=1

if test "x$PROFILE" != "x"; then
  FLAGS="-fprofile-arcs -ftest-coverage"
else
  FLAGS=""
fi

vm/test/cxxtest/cxxtestgen.pl --error-printer --have-eh --abort-on-fail --include=string.h -o vm/test/one.cpp "$*"

g++ $FLAGS -g -Wall -Werror -Ivendor/libtommath -Ivm/test/cxxtest -I. -Ivm -o vm/test/one vm/test/one.cpp && ./vm/test/one
