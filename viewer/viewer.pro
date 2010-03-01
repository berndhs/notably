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
  DEFIENS += DELIBERATE_TEST=1
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
	../ui/helpbox.ui \
	../ui/changefile.ui \
	../ui/notetags.ui \
	../ui/bookdialog.ui \
	../ui/bookrefs.ui \
	../ui/getstring.ui \
	../ui/bookeditor.ui \
	../ui/tageditor.ui \
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
	../src/helpbox.h 
	

