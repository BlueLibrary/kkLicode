#!/usr/bin/env bash

set -e

SCRIPT=`pwd`/$0
FILENAME=`basename $SCRIPT`
PATHNAME=`dirname $SCRIPT`
ROOT=$PATHNAME
BUILD_DIR=$ROOT/build
CURRENT_DIR=`pwd`


LIB_DIR=$BUILD_DIR/libdeps
PREFIX_DIR=$LIB_DIR/build/
FAST_MAKE=''
export ERIZO_HOME=$ROOT/erizo
#echo $ERIZO_HOME
cd servererizo
make