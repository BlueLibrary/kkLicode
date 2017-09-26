#!/usr/bin/env bash

set -e

SCRIPT=`pwd`/$0
FILENAME=`basename $SCRIPT`
PATHNAME=`dirname $SCRIPT`
ROOT=$PATHNAME
export BUILD_DIR=$ROOT/build
CURRENT_DIR=`pwd`


LIB_DIR=$BUILD_DIR/libdeps
PREFIX_DIR=$LIB_DIR/build/
FAST_MAKE=''
export ERIZO_HOME=$ROOT/erizo
echo $ERIZO_HOME
echo $BUILD_DIR/libdeps/build/include
cd servererizo
make