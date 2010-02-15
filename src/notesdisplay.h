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
#include "nota-editbox.h"
#include <ui_mainwin.h>
#include "notaconf.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

/** @brief NotesDisplay class for the Notably note taker program
*/

namespace nota {

class NotesDisplay : public QMainWindow, public Ui_MainWindow {
Q_OBJECT

public:

  NotesDisplay ();
  
  void SetApplication (QApplication *pA);
  void SetConf        (NotaConf & conf);
  
  void dropEvent (QDropEvent * event);
  
public slots:

  void quit ();

private:

  void ReportText ();
  void FakeSaveText ();
  void MakeTables ();
  void MakeTable (QString table);
  
  
  void OpenDB ();
  void WriteDB (const qint64 id, 
                const QString & name,
                const QString & text);
  void CloseDB ();

  QApplication *pApp;
  
  NotaConf     *pConf; 
  QSqlDatabase  db;
  QString       mConName;
  
  QAction    *exitAction;

};

}


#endif
