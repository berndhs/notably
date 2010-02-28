#include "book-pick.h"


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
#include "delib-debug.h"
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>

namespace nota {

BookPick::BookPick (QWidget *parent)
:QDialog (parent)
{
  setupUi (this);
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (selectButton, SIGNAL (clicked()), this, SLOT (accept()));
  
  connect (bookTable, SIGNAL (cellActivated(int,int)),
            this, SLOT (PickedCell (int,int)));
}

void
BookPick::SetFlags (QTableWidgetItem *item)
{
  int flags = item->flags();
  flags &= ~(Qt::ItemIsEditable | Qt::ItemIsDropEnabled );
  flags |= Qt::ItemIsSelectable;
  item->setFlags (static_cast<Qt::ItemFlag>(flags));
}

int
BookPick::Exec ()
{
  selectedSomething = false;
  selectedTitle = QString("");
  ListBooks ();
  return exec ();
}

bool
BookPick::SelectedBook ()
{
  return selectedSomething;
}

QString
BookPick::TitleSelected ()
{
  return selectedTitle;
}

void
BookPick::ListBooks ()
{
  bookTable->clearContents ();
  QString qryStr ("select bookname, description from books where 1");
  QSqlQuery query (*pDB);
  query.exec (qryStr);
  int namendx = query.record().indexOf("bookname");
  int descndx = query.record().indexOf("description");
  int row(0);
  QString name;
  QString desc;
  int nrows = bookTable->rowCount();
  QTableWidgetItem *item;
  while (query.next()) {
    name = query.value (namendx).toString();
    desc = query.value (descndx).toString();
    item = new QTableWidgetItem (name);
    SetFlags (item);
    if (row >= nrows) {
      bookTable->insertRow(row);
      nrows = bookTable->rowCount();
    }
    bookTable->setItem (row,0,item);
    item = new QTableWidgetItem (desc);
    SetFlags (item);
    bookTable->setItem (row,1,item);
    row++;
  }
  
}

void
BookPick::PickedCell (int row, int col)
{
  bookTable->selectRow (row);
  QTableWidgetItem *item = bookTable->item(row,0);
  if (item) {
    selectedTitle = item->text();
    selectedSomething = true;
  } else {
    selectedTitle = "";
    selectedSomething = false;
  }
}

} // namespace

