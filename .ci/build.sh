#!/bin/bash

case $OS in
  linux)
  {
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_PREFIX_PATH=$HOME/Qt/$QTVER/gcc_64 ../
  } ;;  
  macOS)
  {
    PATH=$PATH:$HOME/Qt/$QTVER/clang_64/bin cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE ../
  } ;;
esac