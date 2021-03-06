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

DEFINES += DELIBERATE_QTM1=$$QT_MAJOR_VERSION
DEFINES += DELIBERATE_QTM2=$$QT_MINOR_VERSION
DEFINES += DELIBERATE_QTP=$$QT_PATCH_VERSION

CONFIG(debug, debug|release) {
  DEFINES += DELIBERATE_DEBUG=1
  TARGET = ../bin/notablyd
}

CONFIG(release, debug|release) {
  DEFINES += DELIBERATE_DEBUG=0
  TARGET = ../bin/notably
}


QT += gui core sql webkit

MOC_DIR = tmp/moc
RCC_DIR = tmp/rcc
UI_DIR = tmp/ui
OBJECTS_DIR = tmp/obj

TRANSLATIONS = notably_de.ts

RESOURCES += \
	../nota.qrc

FORMS = \
	../ui/mainwin.ui \
	../ui/helpwin.ui \
	../ui/helpbox.ui \
	../ui/changefile.ui \
	../ui/notetags.ui \
	../ui/bookdialog.ui \
	../ui/bookrefs.ui \
	../ui/getstring.ui \
	../ui/bookeditor.ui \
	../ui/tageditor.ui \
	../ui/fancysearch.ui \
	../ui/import.ui \
	../ui/importlog.ui \
	../ui/tagsearch.ui 


SOURCES = \
        ../src/deliberate.cpp \
        ../src/notesdisplay.cpp \
	../src/delib-debug.cpp \
	../src/notaconf.cpp \
	../src/nota-editbox.cpp \
	../src/version.cpp \
	../src/notemenu.cpp \
	../src/editmenu.cpp \
	../src/special-menu.cpp \
	../src/managemenu.cpp \
	../src/db-manage.cpp \
	../src/content-menu.cpp \
	../src/nota-help.cpp \
	../src/helpbox.cpp \
	../src/notetag-edit.cpp \
	../src/note-book-edit.cpp \
	../src/tagsearch.cpp \
	../src/book-pick.cpp \
	../src/book-edit.cpp \
	../src/tag-edit.cpp \
	../src/export-html.cpp \
	../src/fancy-search.cpp \
	../src/import-export.cpp \
	../src/notably.cpp

HEADERS = \
        ../src/deliberate.h \
	../src/utility-types.h \
	../src/notesdisplay.h \
	../src/delib-debug.h \
	../src/nota-editbox.h \
	../src/version.h \
	../src/notemenu.h \
	../src/editmenu.h \
	../src/special-menu.h \
	../src/managemenu.h \
	../src/db-manage.h \
	../src/content-menu.h \
	../src/notaconf.h \
	../src/nota-help.h \
	../src/notetag-edit.h \
	../src/note-book-edit.h \
	../src/tagsearch.h \
	../src/book-pick.h \
	../src/book-edit.h \
	../src/tag-edit.h \
	../src/export-html.h \
	../src/fancy-search.h \
	../src/import-export.h \
	../src/helpbox.h 
	

