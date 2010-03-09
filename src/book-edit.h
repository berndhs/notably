#ifndef BOOK_EDIT_H
#define BOOK_EDIT_H


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

#include "deliberate.h"
#include "ui_bookeditor.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QTableWidgetItem>

namespace nota {

enum BookState { Book_None = 0, 
                 Book_Old, 
                 Book_New, 
                 Book_Changed,
                 Book_Delete, 
                 Book_High
                };

class BookEdit  : public QDialog, public Ui_BookEditor {
Q_OBJECT

public:

  BookEdit (QWidget * parent);
  
  void SetDB (QSqlDatabase & db) { pDB = &db; }
  
  int  Exec ();
  void Setup ();
  
  
public slots:

  void PickedCell (int row, int col);
  void ChangedCell (int row, int col);
  void Delete ();
  void Add ();
  void Save ();
  void DeleteFromDB (int row);
  void AddToDB (int row);
  
private:

  void GetAllBooks ();
  void SetNoEdit (QTableWidgetItem *item);
  void SetBookState (int row, BookState state);
  
  BookState GetBookState (int row);


  QSqlDatabase  *pDB;
  
  int statusCol;
  int nameCol;
  int descCol;
  
  bool loading;
  bool changingStatus;
  
  QString bookStatus[Book_High];

};

} // namespace


#endif
