#!/bin/bash

LLVM_TAR=llvm-3.4.2-x86_64-unknown-linux-gnu-4.7.tar.bz2

curl -o $LLVM_TAR https://s3.amazonaws.com/asset.rubini.us/prebuilt/$LLVM_TAR \
  && mkdir -p vendor/llvm \
  && tar -C vendor/llvm -xjf $LLVM_TAR
