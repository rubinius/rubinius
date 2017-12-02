#!/bin/bash

export RBX_SUPRESS_DEPRECATION=1

./configure $@

rake -q
