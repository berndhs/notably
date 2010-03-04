#include "book-edit.h"

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

#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QString>
#include <QList>
#include <QVariant>
#include "delib-debug.h"

namespace nota {

BookEdit::BookEdit (QWidget *parent) 
:QDialog(parent),
 pDB (0),
 statusCol(0),
 nameCol(1),
 descCol(2),
 loading (false)
{
  setupUi (this);
  Setup ();
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (deleteButton, SIGNAL (clicked()), this, SLOT (Delete()));
  connect (addButton, SIGNAL (clicked()), this, SLOT (Add()));
  connect (saveButton, SIGNAL (clicked()), this, SLOT (Save()));
  connect (bookTable, SIGNAL (cellActivated(int,int)),
            this, SLOT (PickedCell (int,int)));
  connect (bookTable, SIGNAL (cellChanged (int, int)),
            this, SLOT (ChangedCell (int,int)));
}

void
BookEdit::Setup ()
{
  bookStatus[Book_None] = tr("bad status");
  bookStatus[Book_Old]  = tr("present");
  bookStatus[Book_New]  = tr("new");
  bookStatus[Book_Changed] = tr("changed");
  bookStatus[Book_Delete] = tr("delete");
}

int
BookEdit::Exec ()
{
  GetAllBooks ();
  return exec ();
}

void
BookEdit::GetAllBooks ()
{
  loading = true;
  bookTable->clearContents ();
  bookTable->setRowCount (0);
  if (pDB) {
    QString bookQry ("select bookname, description from books where 1");
    QSqlQuery query (*pDB);
    QString   name, desc;
    int       row(0);
    int       nrows = bookTable->rowCount();
    query.exec (bookQry);
    int       namendx = query.record().indexOf("bookname");
    int       descndx = query.record().indexOf("description");
    QTableWidgetItem *statusItem(0), *nameItem(0), *descItem(0);
    while (query.next()) {
      statusItem = new QTableWidgetItem (bookStatus[Book_Old]);
      SetNoEdit (statusItem);
      name = query.value(namendx).toString();
      nameItem = new QTableWidgetItem (name);
      SetNoEdit (nameItem);
      desc = query.value(descndx).toString();
      descItem = new QTableWidgetItem (desc);
      if (row >= nrows) {
        bookTable->insertRow(row);
        nrows = bookTable->rowCount();
      }
      bookTable->setItem (row,statusCol,statusItem);
      bookTable->setItem (row,nameCol,nameItem);
      bookTable->setItem (row,descCol,descItem);
      row++;
    }
  }
  loading = false;
}

void
BookEdit::SetNoEdit (QTableWidgetItem *item)
{
  int flags = item->flags();
  flags &= ~(Qt::ItemIsEditable | Qt::ItemIsDropEnabled );
  flags |= Qt::ItemIsSelectable;
  item->setFlags (static_cast<Qt::ItemFlag>(flags));
}

void
BookEdit::PickedCell (int row, int col)
{
  QTableWidgetItem *item(0);
  QTableWidgetItem *statusItem(0);
  item = bookTable->item (row,col);
  statusItem = bookTable->item(row,statusCol);
  if ((item !=0) && (statusItem !=0)) {
    if (statusItem->text() == bookStatus[Book_Delete]) {
      deleteButton->setText (tr("Un-Delete"));
    } else {
      deleteButton->setText (tr("Delete"));
    }
  }
}

void
BookEdit::ChangedCell (int row, int col)
{
  if (!loading) {
    QTableWidgetItem * item = bookTable->item (row, col);
    if (item) {
      SetBookState (row, Book_Changed);
    }
  }
}

void
BookEdit::SetBookState (int row, BookState state)
{
  if (state < Book_None || state > Book_High) {
    return;
  }
  QTableWidgetItem *statusItem = bookTable->item(row,statusCol);
  if (statusItem) {
    statusItem->setText (bookStatus[state]);
  }
}

BookState
BookEdit::GetBookState (int row)
{
  QTableWidgetItem * stateItem = bookTable->item (row, statusCol);
  if (stateItem) {
    QString stateName = stateItem->text();
    int stateNum;
    for (stateNum = Book_None; stateNum < Book_High; stateNum++) {
      if (stateName == bookStatus[stateNum]) {
        return static_cast<BookState>(stateNum);
      }
    }
  }
  return Book_None;
}


void
BookEdit::Delete ()
{
  QList<QTableWidgetItem*> ditchThese = bookTable->selectedItems();
  QList<QTableWidgetItem*>::iterator it;
  int row;
  BookState oldstate(Book_None), newstate(Book_Delete);
  for (it=ditchThese.begin(); it != ditchThese.end(); it++) {
    row = (*it)->row();
    oldstate = GetBookState (row);
    if (oldstate == Book_Delete) {
      newstate = Book_New;
    } else {
      newstate = Book_Delete;
    }
    SetBookState ((*it)->row(), newstate);
  }
}

void
BookEdit::Add ()
{
  int newrow = bookTable->rowCount();
  bookTable->insertRow (newrow);
  bookTable->selectRow (newrow);
  QTableWidgetItem * statusItem = new QTableWidgetItem (bookStatus[Book_New]);
  QTableWidgetItem * nameItem = new QTableWidgetItem (tr("new title"));
  QTableWidgetItem * descItem = new QTableWidgetItem (tr("new description"));
  SetNoEdit (statusItem);
  bookTable->setItem (newrow, statusCol, statusItem);
  bookTable->setItem (newrow, nameCol, nameItem);
  bookTable->setItem (newrow, descCol, descItem);
}

void
BookEdit::Save ()
{
  /** make two passes:
      (1) delete all deleted books, AND all their corresponding entries
          in the bookrefs table
      (2) insert all new books
      This has to be done in this order, in case of name collisions
  */
  int row;
  BookState state;
  for (row = 0; row < bookTable->rowCount(); row++) {
    state = GetBookState (row);
    if (state == Book_Delete) {
      DeleteFromDB (row);
    }
  }
  for (row = 0; row < bookTable->rowCount(); row++) {
    state = GetBookState (row);
    if (state == Book_New || state == Book_Changed) {
      AddToDB (row);
    }
  }
  GetAllBooks ();
}

void
BookEdit::DeleteFromDB (int row)
{
  QString bookname;
  QTableWidgetItem * nameItem = bookTable->item(row,nameCol);
  QString qryPattern;
  QString qryText;
  QSqlQuery delQuery (*pDB);
  if (nameItem)  {
    bookname = nameItem->text();
    QString qryPattern ("delete from bookrefs where bookname = '%1'");
    qryText = qryPattern.arg(bookname);
    delQuery.exec (qryText);
    qryPattern = QString ("delete from books where bookname = '%1'");
    qryText = qryPattern.arg(bookname);
    delQuery.exec (qryText);
  }
}

void
BookEdit::AddToDB (int row)
{
  QTableWidgetItem * nameItem = bookTable->item(row, nameCol);
  QTableWidgetItem * descItem = bookTable->item(row, descCol);
  if (nameItem && descItem) {
    QString name = nameItem->text();
    QString desc = descItem->text();
    QString qryText ("insert or replace into books (bookname, description) "
                        " values (?,?)");
    QSqlQuery insertQuery (*pDB);
    insertQuery.prepare (qryText);
    QVariant nameVal;
    QVariant descVal;
    nameVal.setValue (name);
    descVal.setValue (desc);
    insertQuery.bindValue (0,nameVal);
    insertQuery.bindValue (1,descVal);
    insertQuery.exec ();
  }
}

} // namespace


