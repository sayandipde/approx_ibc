#!/bin/sh

export CLANG=/home/sajid/apps/llvm/bin/clang:$CLANG
export LLVM_CONFIG=/home/sajid/apps/llvm/bin/llvm-config:$LLVM_CONFIG
export PKG_CONFIG_PATH=/usr/lib/pkgconfig/:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=/usr/lib:/home/sajid/apps/Halide/bin:/home/sajid/Desktop/apps/Halide/distrib/bin:$LD_LIBRARY_PATH
