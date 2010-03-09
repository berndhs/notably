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

#include "delib-debug.h"
#include <QMainWindow>
#include <QApplication>
#include "utility-types.h"
#include "nota-editbox.h"
#include "notetag-edit.h"
#include "note-book-edit.h"
#include "ui_mainwin.h"
#include "notaconf.h"
#include "notemenu.h"
#include "editmenu.h"
#include "special-menu.h"
#include "managemenu.h"
#include "db-manage.h"
#include "content-menu.h"
#include "version.h"
#include "helpbox.h"
#include "nota-help.h"
#include "export-html.h"

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
#include <QStack>

/** @brief NotesDisplay class for the Notably note taker program
*/

namespace nota {

class NotesDisplay : public QMainWindow, public Ui_MainWindow {
Q_OBJECT

public:

  NotesDisplay (QApplication &app);
  
  void SetApplication (QApplication *pA);
  void SetConf        (NotaConf & conf);
  void Start ();
  
  void dropEvent (QDropEvent * event);
  void mousePressEvent (QMouseEvent *event );
  void mouseReleaseEvent (QMouseEvent *event );
  void resizeEvent ( QResizeEvent * event );
  
public slots:

  void quit ();
  void AboutToQuit ();
  void UserPicked (QListWidgetItem *item);
  void SaveCurrent ();
  void PublishCurrent ();
  void DeleteCurrent ();
  void NewNote ();
  void NameChanged (const QString & name);
  void Help ();
  void HelpHelp ();
  void LicenseHelp ();
  void OnlineHelp ();
  void ShowNothing ();
  void ShowNoteMenu ();
  void ScheduleManage ();
  void ScheduleContent ();
  void ScheduleSpecial ();
  void DoSpecial ();
  void DoContent ();
  void ShowManage ();
  void DebugCheck ();
  void ExecNoteMenu ();
  void ScheduleEdit ();
  void ExecEditMenu ();
  void GrabHtml ();
  void GrabLink ();
  void DoNoteTags ();
  void DoNoteBooks ();
  void ImageInserted (QString imgname);
  void SelectionMade (NoteIdSetType & idset);
  void SelectionMade ();
  void InterNoteLink (qint64 nextnote, bool forward = true);
  void Back ();
  void EditBoxChanged ();
  void CopyNoteLink ();
  void PasteNoteLink ();
  void ExportBook (QString bookname);
  
  
  void ReloadDB ();
  
  void ToggleFont (const FontProperty);
  
  void ShootScreen (const bool whole);
  void DoShootScreen ();

private:

  void SetupMenu ();
  void SetupEdit ();
  void ShutdownClean ();
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
  void GetTagPix (const QString tagname, int t);
  void CopyPageImages (const qint64 notesid, 
                   const QString srcdir,
                   const QString destdir);
  void OpenDB ();
  void WriteNote (const qint64 id, 
                const QString & name,
                const QString & text);
  void CloseDB ();
  
  void FillNotesList (  QListWidget *notesIndex);
  void FillNotesList ( QListWidget * notesIndex, NoteIdSetType & idset);
  
  QListWidgetItem * ListThisNote (  QListWidget *notesIndex,
                                  const qint64    id,
                                  const QString   &name);

  QApplication *pApp;
  
  bool          didShutdownActions;
  
  NotaConf     *pConf; 
  NoteMenu      noteMenu;
  EditMenu      editMenu;
  SpecialMenu   specialMenu;
  ManageMenu    manageMenu;
  ContentMenu   contentMenu;
  HelpBox       helpBox;
  NotaHelp      helpBrowser;
  NoteTagEdit   noteTagEditor;
  NoteBookEdit  noteBookEditor;
  ExportHtml    htmlExporter;
  
  DBManage      dbManager;
  QSqlDatabase  db;
  QString       mConName;
  
  qint64     currentId;
  QString    currentName;
  QString    newName;
  QListWidgetItem  *curItem;
  QStack<qint64>    idStack;
  bool       isChanged;
  QString    noteLinkCopy;
  
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
  QAction    *noteMenuAction;
  QAction    *helpAction;
  QAction    *editAction;
  QAction    *specialAction;
  QAction    *manageAction;
  QAction    *contentAction;
  
  QShortcut  *exitShort;
  QShortcut  *noteMenuShort;
  QShortcut  *editShort;
  QShortcut  *specialShort;
  QShortcut  *saveShort;
  QShortcut  *manageShort;
  QShortcut  *helpShort;
  QShortcut  *contentShort;
  
  QTimer     debugTimer;

};

}


#endif
