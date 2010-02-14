#include "delib-debug.h"
#include <stdlib.h>
#include <qapplication.h>

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


using namespace std;

namespace deliberate {

void UseMyOwnMessageHandler ()
{
  qInstallMsgHandler (deliberate::MyOwnMessageOutput);
}

QTextStream & StdOut ()
{
  static QTextStream *out(0);
  
  if (out == 0) {
    out = new QTextStream (stdout);
  }
  return *out;
}
void MyOwnMessageOutput (QtMsgType type, const char* msg)
{
#if DELIBERATE_DEBUG
  switch (type) {
  case QtDebugMsg:
    StdOut () << "Qt Debug: " << msg << endl;
    break;
  case QtWarningMsg:
    StdOut ()  << "Qt Warn: " << msg << endl;
    break;
  case QtCriticalMsg:
    StdOut ()  << "Qt Critical: " << msg << endl;
    break;
  case QtFatalMsg:
    StdOut ()  << "Qt Fatal: " << msg << endl;
    abort();
    break;
  default:
    StdOut ()  << " unknown Qt msg type: " << msg << endl;
    break;
  }
#else
  switch (type) {
  case QtFatalMsg:
    StdOut ()  << "Qt Fatal: " << msg << endl;
    abort();
    break;
  case QtDebugMsg:
  case QtWarningMsg:
  case QtCriticalMsg:
  default:
    // start prayer, maybe it's not a problem
    break;
  }
#endif

}

}
