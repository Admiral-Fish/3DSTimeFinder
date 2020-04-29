#!/bin/bash

case $OS in
  linux)
  {
    zip -r 3DSTimeFinder-linux.zip 3DSTimeFinder
    sha256sum 3DSTimeFinder-linux.zip > 3DSTimeFinder-linux.zip.sha256
  } ;;  
  macOS)
  {
    macdeployqt 3DSTimeFinder.app -dmg -verbose=2
    zip -r 3DSTimeFinder-macOS.zip 3DSTimeFinder.dmg
    shasum -a 256 3DSTimeFinder-macOS.zip > 3DSTimeFinder-macOS.zip.sha256
  } ;;
esac