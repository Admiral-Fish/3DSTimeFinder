#!/bin/bash

case $OS in
  linux)
  {
    source /opt/qt5*/bin/qt5*-env.sh
    qmake 3DSTimeFinder.pro
    make -j $(nproc)
  } ;;  
  macOS)
  {
    qmake 3DSTimeFinder.pro
    make -j $(sysctl -n hw.physicalcpu)
  } ;;
esac