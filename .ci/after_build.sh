#!/bin/bash

case $OS in
  linux)
  {
    mv build/Source/Forms/3DSTimeFinder .
    tar czf 3DSTimeFinder-linux.tar.gz 3DSTimeFinder
    sha256sum 3DSTimeFinder-linux.tar.gz > 3DSTimeFinder-linux.tar.gz.sha256
  } ;;  
  macOS)
  {
    mv build/Source/Forms/3DSTimeFinder.app .
    PATH=$PATH:$HOME/Qt/$QTVER/macos/bin macdeployqt 3DSTimeFinder.app -no-plugins -verbose=3

    # Fix rpath
    install_name_tool -rpath /Users/appveyor/Qt/$QTVER/macos/lib @executable_path/../Frameworks 3DSTimeFinder.app/Contents/MacOS/3DSTimeFinder

    # Create plugins folder
    mkdir 3DSTimeFinder.app/Contents/PlugIns
    mkdir 3DSTimeFinder.app/Contents/PlugIns/platforms
    mkdir 3DSTimeFinder.app/Contents/PlugIns/styles

    # Copy over plugins we need
    cp $HOME/Qt/$QTVER/macos/plugins/platforms/libqcocoa.dylib 3DSTimeFinder.app/Contents/PlugIns/platforms
    cp $HOME/Qt/$QTVER/macos/plugins/styles/libqmacstyle.dylib 3DSTimeFinder.app/Contents/PlugIns/styles

    tar czf 3DSTimeFinder-macOS.tar.gz 3DSTimeFinder.app
    shasum -a 256 3DSTimeFinder-macOS.tar.gz > 3DSTimeFinder-macOS.tar.gz.sha256
  } ;;
esac