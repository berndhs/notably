#include "notesdisplay.h"
#include "delib-debug.h"

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
//


namespace deliberate {

NotesDisplay::NotesDisplay ()
:pApp(0)
{
  setupUi (this);
  exitAction = new QAction (tr("Exit") , this);
  menubar->addAction (exitAction);
  
  connect (exitAction, SIGNAL (triggered()), this, SLOT (quit()));
}

void
NotesDisplay::SetApplication (QApplication * pA)
{
  pApp = pA;
}

void
NotesDisplay::quit ()
{
  if (pApp) {
    pApp->quit();
  }
  ReportText ();
  StdOut() << " bye for now" << endl;
}

void
NotesDisplay::ReportText ()
{
  StdOut() << editBox->toHtml ();
}

}
