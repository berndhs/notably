
# Makefile for notably
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

PRO_FILE = notably.pro
MAKEFILE = MakeNotably
DESTDIR = /usr/bin

QMAKE = `sh ./findqmake.sh`

all: $(MAKEFILE)
	make -f $(MAKEFILE) all

clean: $(MAKEFILE)
	make -f $(MAKEFILE) clean
	
debug: $(MAKEFILE)
	make -f $(MAKEFILE) debug

release: $(MAKEFILE)
	make -f $(MAKEFILE) release

install:
	sh install.sh $(DESTDIR)

userinstall:
	sh install.sh $(HOME)/bin

qmake: 
	$(QMAKE) $(PRO_FILE)

$(MAKEFILE): $(PRO_FILE)
	$(QMAKE) $(PRO_FILE)
