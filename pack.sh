#!/bin/bash

NAME=notably
VERSION=0.3.4
PACKAGEDIR=${HOME}/packaging/notably

makearchive.sh ${NAME}-${VERSION} master
cp ${NAME}-${VERSION}.tar.gz ${PACKAGEDIR}
ls -l ${PACKAGEDIR}/${NAME}-${VERSION}.tar.gz
