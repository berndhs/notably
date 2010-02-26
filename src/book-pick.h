#ifndef BOOK_PICK_H
#define BOOK_PICK_H
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
#include <QDialog>
#include "ui_bookdialog.h"
#include <QTableWidgetItem>
#include <QSqlDatabase>

namespace nota {


class BookPick : public QDialog, public Ui_BookDialog {

Q_OBJECT

public:

  BookPick (QWidget * parent);
  
  void SetDB (QSqlDatabase & db) { pDB = &db; }
  
  int  Exec ();
  bool SelectedBook ();
  
  QString TitleSelected (); 
  
public slots:

  void PickedCell (int row, int col);
  
  
private:

  void SetFlags (QTableWidgetItem *item);
  void ListBooks ();
  
  QSqlDatabase *pDB;
  
  bool     selectedSomething;
  QString  selectedTitle;
};


} // namespace


#endif

