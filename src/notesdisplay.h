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
#include "notetag-edit.h"
#include <ui_mainwin.h>
#include "notaconf.h"
#include "notemenu.h"
#include "editmenu.h"
#include "managemenu.h"
#include "db-manage.h"
#include "version.h"
#include "helpbox.h"
#include "nota-help.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QListWidgetItem>
#include <QTimer>
#include <QMouseEvent>
#include <QShortcut>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

/** @brief NotesDisplay class for the Notably note taker program
*/

namespace nota {

class NotesDisplay : public QMainWindow, public Ui_MainWindow {
Q_OBJECT

public:

  NotesDisplay ();
  
  void SetApplication (QApplication *pA);
  void SetConf        (NotaConf & conf);
  void Start ();
  
  void dropEvent (QDropEvent * event);
  void mousePressEvent (QMouseEvent *event );
  void mouseReleaseEvent (QMouseEvent *event );
  void resizeEvent ( QResizeEvent * event );
  
public slots:

  void quit ();
  void UserPicked (QListWidgetItem *item);
  void SaveCurrent ();
  void PublishCurrent ();
  void DeleteCurrent ();
  void NewNote ();
  void NameChanged (const QString & name);
  void Help ();
  void HelpHelp ();
  void LicenseHelp ();
  void ShowNothing ();
  void ShowNoteMenu ();
  void ScheduleManage ();
  void ShowManage ();
  void DebugCheck ();
  void ExecNoteMenu ();
  void ScheduleEdit ();
  void ExecEditMenu ();
  void GrabHtml ();
  void GrabLink ();
  void DoNoteTags ();
  
  void ReloadDB ();
  
  void ToggleFont (const FontProperty);
  
  void ShootScreen (const bool whole);
  void DoShootScreen ();

private:

  void SetupMenu ();
  void SetupEdit ();
  void ReportText ();
  void MakeTables ();
  void MakeTable (QString table);
  bool DBExists ();
  void DeleteFromDB (const qint64 id);
  void MakeNew  (qint64 & id, QString & name);
  void ShowNote (QListWidgetItem * item,
                 const qint64 id, 
                 const QString & name);
  
  void ListTags (const qint64 noteid);
  void GetTagPix (const QString tagname, QPixmap & pix);
  void OpenDB ();
  void WriteNote (const qint64 id, 
                const QString & name,
                const QString & text);
  void CloseDB ();
  
  void FillNotesList (  QListWidget *notesIndex);
  
  QListWidgetItem * ListThisNote (  QListWidget *notesIndex,
                                  const qint64    id,
                                  const QString   &name);

  QApplication *pApp;
  
  NotaConf     *pConf; 
  NoteMenu      noteMenu;
  EditMenu      editMenu;
  ManageMenu    manageMenu;
  HelpBox       helpBox;
  NotaHelp      helpBrowser;
  NoteTagEdit   noteTagEditor;
  
  DBManage      dbManager;
  QSqlDatabase  db;
  QString       mConName;
  
  qint64     currentId;
  QString    currentName;
  QString    newName;
  QListWidgetItem  *curItem;
  
  QPoint     corner1;
  QPoint     corner2;
  
  QLabel     noLabel;
  QLabel     tagLabel[5];
  QPixmap    tagPix[5];
  int        maxTags;
  int        numTags;
  QPixmap    noTagPix;
  
  bool       isNew;
  bool       nameChanged;
  bool       showingNote;
  bool       fontProperty[FP_max];
  
  QAction    *exitAction;
  QAction    *saveAction;
  QAction    *noteMenuAction;
  QAction    *helpAction;
  QAction    *editAction;
  QAction    *manageAction;
  
  QShortcut  *exitShort;
  QShortcut  *noteMenuShort;
  QShortcut  *editShort;
  QShortcut  *saveShort;
  QShortcut  *manageShort;
  QShortcut  *helpShort;
  
  QTimer     debugTimer;

};

}


#endif
