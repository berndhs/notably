#include "version.h"
#include <QMessageBox>
#include <QTimer>

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

namespace deliberate {

  QString ProgramVersion("0.1");

  QString ProgramName("Notably");
  QString copyright ("Copyright (C) 2010 Bernd H Stramm");
  
  QString Version ()
  { 
    return QString(ProgramName + " Version " 
                   + ProgramVersion + " " + QString(__DATE__) + " "
		       + QString(__TIME__)
		       + QString("\r\n")
		       + copyright);
  }

  void ShowVersionWindow ()
  {
    QString versionMessage(Version());
    QMessageBox box;
    box.setText (versionMessage);
    QTimer::singleShot(30000,&box,SLOT(accept()));
    box.exec();
  }
  
  void CLIVersion ()
  {
    StdOut() << Version() << endl;
  }
  
  QString MyName()
  { 
    return ProgramName;
  }

}
