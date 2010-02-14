#ifndef NOTESDISPLAY_H
#define NOTESDISPLAY_H

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

#include <QMainWindow>
#include <QApplication>
#include <ui_mainwin.h>

/** @brief NotesDisplay class for the Notably note taker program
*/

namespace deliberate {

class NotesDisplay : public QMainWindow, public Ui_MainWindow {
Q_OBJECT

public:

  NotesDisplay ();
  
  void SetApplication (QApplication *pA);
  
public slots:

  void quit ();

private:

  void ReportText ();

  QApplication *pApp;
  
  QAction    *exitAction;

};

}


#endif
