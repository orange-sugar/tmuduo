#!/bin/sh

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-build}
BUILD_TYPE=${BUILD_TYPE:-release}
INSTALL_DIR=${INSTALL_DIR:-./$BUILD_TYPE}

CXX=${CXX:-clang++}

ln -sf $BUILD_DIR/$BUILD_TYPE/compile_commands.json

mkdir -p $BUILD_DIR/$BUILD_TYPE
cd $BUILD_DIR/$BUILD_TYPE 
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      ${SOURCE_DIR} \
  && make $*
  
# cp compile_commands.json ../compile_commands.json 