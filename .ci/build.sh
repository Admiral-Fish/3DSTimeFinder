#!/bin/bash

case $OS in
  linux)
  {
    $HOME/Qt/5.14/gcc_64/bin/qmake 3DSTimeFinder.pro
    make -j $(nproc)
  } ;;  
  macOS)
  {
    qmake 3DSTimeFinder.pro
    make -j $(sysctl -n hw.physicalcpu)
  } ;;
esac