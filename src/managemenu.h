#ifndef MANAGEMENU_H
#define MANAGEMENU_H
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
#include <QWidget>
#include <QMenu>
#include <QFileDialog>
#include <QSqlDatabase>
#include "delib-debug.h"
#include "notaconf.h"
#include "ui_changefile.h"
#include "book-edit.h"
#include "book-pick.h"
#include "tag-edit.h"
#include "export-html.h"

namespace nota {

class ManageMenu : public QWidget {
Q_OBJECT

public:

  ManageMenu (QWidget *parent);
  
  void SetConf (NotaConf * pC) { pConf = pC; }
  void SetDB (QSqlDatabase &db);
  
  void Exec (QPoint here);
  
public slots:

  void ChangeFilename ();
  void SaveLoc ();
  void CancelLoc ();
  void ExportAll ();
  void EditBooks ();
  void EditTags ();
  void ExportBook ();
  void ExportBookSql ();
  void MergeOtherDB ();
  void ImportBusy (bool busy);
  void GetNewFilename ();
  
signals:

  void SigReload ();
  void SigExportBook (QString bookname);
  void SigExportBookSql (QString bookname, QString destfile);
  void SigExportImages (QString destDBName);
  void SigMerge (QString path);

private:

  void ConnectDialogs ();
  QMenu   menu;
  Ui_ChangeFile   fileUI;
  QDialog         fileDialog;
  
  BookEdit        bookEditor;
  TagEdit         tagEditor;
  BookPick        bookPicker;
  
  
  NotaConf  *pConf;
  
  QAction  *fileNameAction;
  QAction  *exportAction;
  QAction  *bookAction;
  QAction  *bookExSqlAction;
  QAction  *tagAction;
  QAction  *htmlAction;
  QAction  *mergeAction;
  
};

}


#endif

