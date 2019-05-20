#!/bin/bash

export RBX_SUPRESS_DEPRECATION=1

for f in $(find build/libraries/gems/ -name extconf.rb)
do
  pushd $(dirname $f) && make clean
  popd
done

./configure $@

rake -q
