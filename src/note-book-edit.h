#ifndef NOTE_BOOK_EDIT_H
#define NOTE_BOOK_EDIT_H

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
#include "utility-types.h"
#include "ui_bookrefs.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QTableWidgetItem>
#include <map>

namespace nota {


class NoteBookEdit : public QDialog, public Ui_BookRefs {

Q_OBJECT

public:

  NoteBookEdit (QWidget *parent = 0);
  
  void SetDB (QSqlDatabase & thedb) { db = &thedb; }
  
  void ManageBooks (const qint64 noteid, 
                   const QString& notename);
 
  void GetBooknames (const qint64 noteid, QStringList & names);  
  
private slots:

  void Done ();
  void PickedAvailBook (int row, int col);
  void PickedHaveBook  (QListWidgetItem *item);

  
private:

  void GetAllBooks ();
  void FillBookList (QListWidget * list, QStringList & books);
  void CollectNames (QListWidget * list, QStringList & strings);
  void SetFlags (QTableWidgetItem *item);
  
  qint64  noteId;

  QSqlDatabase  *db;
  QStringList   allbooks;
  
  
};

}

#endif
