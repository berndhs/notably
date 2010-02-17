#ifndef SATVIEW_VERSION_H
#define SATVIEW_VERSION_H

//
//  Copyright (C) 2009 - Bernd H Stramm 
//
// This program is distributed under the terms of 
// the GNU General Public License version 3 
//
// This software is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty 
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//

#include <QString>
#include "delib-debug.h"

namespace deliberate {

  QString MyName();
  QString Version() ;
  
  void ShowVersionWindow();
  void CLIVersion();

}

#endif
