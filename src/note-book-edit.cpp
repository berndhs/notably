#include "note-book-edit.h"

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

#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

namespace nota {

NoteBookEdit::NoteBookEdit (QWidget *parent)
:QDialog (parent),
 db(0)
{
  setupUi (this);
  connect (doneButton, SIGNAL (clicked()), this, SLOT (Done()) );
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (noteBooks, SIGNAL (itemActivated (QListWidgetItem *)),
           this, SLOT (PickedHaveBook (QListWidgetItem *)));
  connect (bookTable, SIGNAL (cellActivated (int, int)),
           this, SLOT (PickedAvailBook (int, int)));
}

void
NoteBookEdit::ManageBooks (const qint64 noteid,
                         const QString & notename)
{
  if (db == 0) {
    return;
  }
  noteId = noteid;
  QStringList booknames;
  GetAllBooks ();
  noteName->setText (notename);
  GetBooknames (noteid, booknames);
  FillBookList (noteBooks, booknames);
  exec ();
}

void
NoteBookEdit::GetBooknames ( const qint64 noteid, QStringList & names)
{
  names.clear ();
  if (db == 0) {
    return;
  }
  QString tagsPattern ("select bookname from bookrefs where noteid = %1");
  QString tagsQuery = tagsPattern.arg(QString::number(noteid));
  QSqlQuery query (*db);
  query.exec (tagsQuery);
  QString tagname;
  while (query.next()) {
    names.append (query.value(0).toString());
  }
}

void
NoteBookEdit::PickedHaveBook ( QListWidgetItem * item)
{
  if (item) {
    int itemRow = noteBooks->row (item);
    QListWidgetItem * deadItem = noteBooks->takeItem (itemRow);
    delete deadItem;
  }
}

void
NoteBookEdit::PickedAvailBook (int row, int col)
{
  bookTable->selectRow (row);
  QTableWidgetItem * book = bookTable->item(row,0);
  if (book) {
    QString name = book->text();
    QList <QListWidgetItem*> haveAlready;
    haveAlready = noteBooks->findItems (name,Qt::MatchCaseSensitive );
    if (haveAlready.size() < 1) {
      QListWidgetItem * newBook = new QListWidgetItem (book->text());
      newBook->setText (name);
      noteBooks->addItem (newBook);
    }
  }
}


void
NoteBookEdit::GetAllBooks ()
{
  QString qryStr ("select bookname, description from books where 1");
  QSqlQuery query (*db);
  query.exec (qryStr);
  int nameindex = query.record().indexOf ("bookname");
  int descindex = query.record().indexOf ("description");
  QString name;
  QString desc;
  allbooks.clear ();
  QTableWidgetItem *item;
  bookTable->clear ();
  int row=0;
  int nrows = bookTable->rowCount();
  while (query.next () ) {
    if (row >= nrows) {
      bookTable->insertRow (row);
      nrows = bookTable->rowCount();
    }
    name = query.value (nameindex).toString ();
    desc = query.value (descindex).toString ();
    item = new QTableWidgetItem (name);
    SetFlags (item);
    bookTable->setItem (row,0,item);
    item = new QTableWidgetItem (desc);
    SetFlags (item);
    bookTable->setItem (row, 1, item);
    row++;
  }
}

void
NoteBookEdit::SetFlags (QTableWidgetItem *item)
{
  int flags = item->flags();
  flags &= ~(Qt::ItemIsEditable | Qt::ItemIsDropEnabled );
  flags |= Qt::ItemIsSelectable;
  item->setFlags (static_cast<Qt::ItemFlag>(flags));
}


void
NoteBookEdit::FillBookList (QListWidget *list, QStringList &books)
{
  QStringList::iterator bookit;
  QListWidgetItem      *item(0);
  QString               name;
  
  list->clear ();
  for (bookit = books.begin(); bookit != books.end(); bookit++) {
    item = new QListWidgetItem (list);
    name = *bookit;
    item->setText (name);
  }
}

void
NoteBookEdit::CollectNames (QListWidget *list, QStringList & strings)
{
  strings.clear();
  if (list) {
    QListWidgetItem *item (0);
    for (int r=0; r<list->count(); r++) {
      item = list->item(r);
      if (item) {
        strings.append (item->text());
      }
    }
  }
}

void
NoteBookEdit::Done ()
{
  QString deletePattern ("delete from bookrefs where noteid = %1");
  QString deleteStr = deletePattern.arg(noteId);
  QSqlQuery query (*db);
  query.exec (deleteStr);
  QStringList haveBooks;
  CollectNames (noteBooks, haveBooks);
  QString addPattern ("insert or replace into bookrefs ( noteid, bookname )"
                      " values (%1,'%2')");
  QString addStr;
  QStringList::iterator bookit;
  for (bookit = haveBooks.begin(); bookit != haveBooks.end(); bookit++) {
    addStr = addPattern.arg(noteId).arg (*bookit);
    query.exec (addStr);
  }
  accept ();
}


} // namespace

