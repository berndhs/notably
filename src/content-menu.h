#ifndef CONTENT_MENU_H
#define CONTENT_MENU_H
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

#include "deliberate.h"
#include "notaconf.h"
#include "tagsearch.h"
#include "book-pick.h"
#include "fancy-search.h"
#include <QDialog>
#include "ui_getstring.h"
#include <QSqlDatabase>
#include <QWidget>
#include <QMenu>
#include <QAction>


namespace nota {

class ContentMenu : public QWidget {
Q_OBJECT

public:

  ContentMenu (QWidget *parent);
  
  void SetConf (NotaConf * pC) { pConf = pC; }
  void SetDB (QSqlDatabase & db);
  
  NoteIdSetType  & SelectedNotes ();
  
  void Exec (QPoint here);
  
public slots:

  void TagSearchExec ();
  void SelectAllNotes (bool doemit = true);
  void NotagNotes ();
  void NobookNotes ();
  void Books ();
  void MultiSearch ();
  void DoFancySearch ();
  void DoSearch ();

signals:

  void Selected  (NoteIdSetType & list);
  void DoneSelection ();

private:

  void FindSelectedNotes (NoteIdSetType & results,
                          QStringList & tagnames);
  void SetupSearchbox ();
  void SingleIdQuery (NoteIdSetType & theNoteSet, QString exceptTable);

  NotaConf   *pConf;
  QSqlDatabase *pDB;
  
  QMenu    menu;
  
  QAction   *searchAction;
  QAction   *advancedAction;
  QAction   *bookAction;
  QAction   *nobookAction;
  QAction   *tagsAction;
  QAction   *notagAction;
  QAction   *allAction;
  
  TagSearch  tagSearch ;
  
  BookPick   bookPick;
  
  NoteIdSetType   noteSet;
  
  QDialog searchBox;
  Ui_GetStringDialog   searchUi;
  
  FancySearch  fancySearch;
  
};

}


#endif

