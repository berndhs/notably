#!/bin/sh
# 
# Build Script for drss
#
# //
# //  Copyright (C) 2010 - Bernd H Stramm 
# //
# // This program is distributed under the terms of 
# // the GNU General Public License version 3 
# //
# // This software is distributed in the hope that it will be useful, 
# // but WITHOUT ANY WARRANTY; without even the implied warranty 
# // of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# //


TARGET=notably
DOT_PRO_FILE=$TARGET.pro
LOCALMAKE=Makefile
MAKE=make
BUILD_DIR=viewer

QM=`which qmake`
QM4=`which qmake-qt4`

if [ -z $QM ] 
then
  RES3="0"
else
  RES3=`$QM -query QT_VERSION`
fi
if [ -z $QM4 ]
then
  RES4="0"
else
  RES4=`$QM4 -query QT_VERSION`
fi

MATCH3=`expr match $RES3 '4.'`
MATCH4=`expr match $RES4 '4.'`

QMAKE=qmake

if [ $MATCH3 -eq 2 ]
then
  QMAKE=$QM
else
  if [ $MATCH4 -eq 2 ]
  then
    QMAKE=$QM4
  else
    echo cannot find qmake for Qt4
    exit 1
  fi
fi

echo
echo Using $QMAKE
echo
echo Trying to build Makefile
echo

$QMAKE $DOT_PRO_FILE
OK=$?
if [ $OK -ne 0 ]
then
  echo Failed to build Makefile
  exit 1
fi

$MAKE -f $LOCALMAKE clean
echo
echo Try to compile $TARGET
echo

$MAKE -f $LOCALMAKE release

if [ -e ${BUILD_DIR}/$TARGET ] 
then
  echo
  echo successfully built ${BUILD_DIR}/$TARGET
  echo
  ls -l ${BUILD_DIR}/$TARGET
else
  echo 
  echo Could not build ${BUILD_DIR}/$TARGET
  echo
fi

