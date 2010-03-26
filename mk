#!/bin/sh

MAKEFILE=MakeNotably

if [ $# -gt 0 ]
then
  TARG=$1
else
  TARG=""
fi

make -f ${MAKEFILE} ${TARG}

