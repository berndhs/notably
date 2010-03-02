#ifndef DELIB_DEBUG_H
#define DELIB_DEBUG_H

//
//  Copyright (C) 2010 - Bernd H Stramm 
//
// This program is distributed under the terms of 
// the GNU General Public License version 3 
//
// This software is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty 
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//

#include "deliberate.h"

#if DELIBERATE_DEBUG
#include <QDebug>
#else
#include <QDebug>
#endif

namespace deliberate {


void UseMyOwnMessageHandler ();

void MyOwnMessageOutput (QtMsgType type, const char* msg);


}
#endif
