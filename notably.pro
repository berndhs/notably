#

# Qt4 .pro file for notably
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
#//

TEMPLATE = subdirs

CONFIG += qt debug_and_release
INCLUDEPATH += src

CONFIG(debug, debug|release) {
  DEFINES += DELIBERATE_DEBUG=1
  DEFINES += DELIBERATE_TEST=1
}

CONFIG(release, debug|release) {
  DEFINES += DELIBERATE_DEBUG=0
}

TRANSLATIONS = notablytop_de.ts

SUBDIRS = viewer

RESOURCES += nota.qrc

