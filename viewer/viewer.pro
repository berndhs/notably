#

#//
#//  Copyright (C) 2010 - Bernd H Stramm 
#//
#// This program is distributed under the terms of 
#// the GNU General Public License version 3 
#//
#// This software is distributed in the hope that it will be useful, 
#// but WITHOUT ANY WARRANTY; without even the implied warranty 
#// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
#/

TEMPLATE = app

CONFIG += qt debug_and_release

INCLUDEPATH += ../src

CONFIG(debug, debug|release) {
  DEFINES += DELIBERATE_DEBUG=1
  TARGET = notablyd
}

CONFIG(release, debug|release) {
  DEFINES += DELIBERATE_DEBUG=0
  TARGET = notably
}

QT += gui core sql webkit

RESOURCES += \
	../nota.qrc

FORMS = \
	../ui/mainwin.ui \
	../ui/helpwin.ui \
	../ui/helpbox.ui 


SOURCES = \
        ../src/notesdisplay.cpp \
	../src/delib-debug.cpp \
	../src/notaconf.cpp \
	../src/nota-editbox.cpp \
	../src/version.cpp \
	../src/notemenu.cpp \
	../src/editmenu.cpp \
	../src/nota-help.cpp \
	../src/helpbox.cpp \
	../src/notably.cpp

HEADERS = \
	../src/notesdisplay.h \
	../src/delib-debug.h \
	../src/nota-editbox.h \
	../src/version.h \
	../src/notemenu.h \
	../src/editmenu.h \
	../src/notaconf.h \
	../src/nota-help.h \
	../src/helpbox.h 
	

