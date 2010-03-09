
#include "fancy-search.h"

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

#include <QListWidgetItem>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>

namespace nota {

FancySearch::FancySearch (QWidget *parent)
:QDialog(parent),
 pDB (0)
{
}

void
FancySearch::Setup (QSqlDatabase *db)
{
  pDB = db;
  setupUi (this);
  tagsExcept = false; tagsAll = true; tagsAny = false;
  booksExcept = false; booksAll = true; booksAny = false;
  connect (searchButton, SIGNAL (clicked()), this, SLOT (DoSearch()));
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (moreBooksButton, SIGNAL (clicked()), this, SLOT (AddAllBooks()));
  connect (moreTagsButton, SIGNAL (clicked()), this, SLOT (AddAllTags()));
  connect (tagList, SIGNAL (itemActivated (QListWidgetItem*)),
           this, SLOT (PickedTag (QListWidgetItem*)));
  connect (bookList, SIGNAL (itemActivated (QListWidgetItem*)),
           this, SLOT (PickedBook (QListWidgetItem*)));
}

void
FancySearch::Search ()
{
  if (pDB) {
    didSearch = false;
    AddAllTags ();
    AddAllBooks ();
    SetRadios ();
    exec ();
  }
}

void
FancySearch::SetRadios ()
{
  allTags->setChecked (tagsAll);
  anyTags->setChecked (tagsAny);
  noneTags->setChecked (tagsExcept);
  allBooks->setChecked (booksAll);
  anyBooks->setChecked (booksAny);
  noneBooks->setChecked (booksExcept);
}

void
FancySearch::DoSearch ()
{
  noteSet.clear();
  NoteIdSetType bookSet;
  NoteIdSetType tagSet;
  MakeSet (bookSet, BookCond());
  MakeSet (tagSet, TagCond());
  bool doAnd = connectAnd->isChecked();
  bool doOr = connectOr ->isChecked();
  if (doAnd) {
    noteSet = tagSet & bookSet; // intersect
  } else if (doOr) {
    noteSet = tagSet + bookSet;  // union
  }
 
  QSqlQuery clean (*pDB);
  clean.exec ("drop view if exists temp_book");
  clean.exec ("drop view if exists temp_tag");
  didSearch = true;
  accept ();
}

void
FancySearch::MakeSet (NoteIdSetType &set, QStringList commands)
{ 
  QSqlQuery query (*pDB);
  QStringList::iterator sit;
  set.clear();
  bool ok (false);
  for (sit = commands.begin(); sit != commands.end(); sit++) {
    ok = query.exec (*sit);
  }
  int idndx = 0;
  if (ok) {
    qint64 id;
    while (query.next()) {
      id = query.value(idndx).toLongLong();
      set.insert (id);
    }
  }

}

QStringList
FancySearch::BookCond ()
{
  booksAll = allBooks->isChecked();
  booksAny = anyBooks->isChecked();
  booksExcept = noneBooks->isChecked();
  QString setOp;
  if (booksAll) {
    setOp = " INTERSECT ";
  } else if (booksAny) {
    setOp = " UNION ";
  } 
  QString select ("select noteid from notes where 0");
  QString selectPattern (" select noteid from bookrefs where bookname = '%1' ");
  int numrows = bookList->count();
  int firstrow (numrows+1);
  int r;
  QListWidgetItem *item;
  QString         bookname;
  
  for (r=0; r < numrows; r++) {
    item = bookList->item(r);
    if (item && ! item->isHidden()) {
      bookname = item->text();
      select = selectPattern.arg(bookname);
      firstrow = r;
      break;
    }
  }
  for (r=firstrow+1; r < numrows; r++) {
    item = bookList->item(r);
    bookname = item->text();
    select.append (setOp);
    select.append (selectPattern.arg(bookname));
  }
  QStringList list;
  if (booksExcept) {
    QString temp (" create temp view temp_book as ");
    temp.append (select);
    temp.append ("; ");
    list << temp;
    temp = QString(" select noteid from notes except select noteid from temp_book where 1;");
    list << temp;
  } else {
    list << select;
  }
  return list;
}


QStringList
FancySearch::TagCond ()
{
  tagsAll = allTags->isChecked();
  tagsAny = anyTags->isChecked();
  tagsExcept = noneTags->isChecked();
  QString setOp;  if (tagsAll) {
    setOp = " INTERSECT ";
  } else if (tagsAny) {
    setOp = " UNION ";
  } 
  QString select ("select noteid from notes where 0");
  QString selectPattern (" select noteid from tagrefs where tagname = '%1' ");
  int numrows = tagList->count();
  int firstrow (numrows+1);
  int r;
  QListWidgetItem *item;
  QString         tagname;
  for (r=0; r < numrows; r++) {
    item = tagList->item(r);
    if (item && ! item->isHidden()) {
      tagname = item->text();
      select = selectPattern.arg(tagname);
      firstrow = r;
      break;
    }
  }
  for (r=firstrow+1; r < numrows; r++) {
    item = tagList->item(r);
    tagname = item->text();
    select.append (setOp);
    select.append (selectPattern.arg(tagname));
  }
  QStringList list;
  if (tagsExcept) {
    QString temp (" create temp view temp_tag as ");
    temp.append (select);
    temp.append ("; ");
    list << temp;
    temp = QString(" select noteid from notes except select noteid from temp_tag where 1;");
    list << temp;
  } else {
    list << select;
  }
  if (list.isEmpty()) {
    list << "select noteid from notes where 1";
  }
  return list;
}



void
FancySearch::AddAllTags ()
{
  tagList->clear ();
  QSqlQuery query (*pDB);
  QString qryStr ("select tagname, icon from tags where 1");
  query.exec (qryStr);
  int namendx = query.record().indexOf ("tagname");
  int iconndx = query.record().indexOf ("icon");
  QString name;
  QPixmap *pix;
  QByteArray  pixBytes;
  QListWidgetItem *item;
  while (query.next()) {
    name = query.value(namendx).toString();
    item = new QListWidgetItem (tagList);
    item->setText (name);
    pixBytes = query.value(iconndx).toByteArray();
    pix = new QPixmap;
    pix->loadFromData (pixBytes);
    item->setIcon (QIcon (*pix));
  }
}

void
FancySearch::AddAllBooks ()
{
  bookList->clear ();
  QSqlQuery query (*pDB);
  QString qryStr ("select bookname from books where 1");
  query.exec (qryStr);
  int namendx = query.record().indexOf ("bookname");
  QString name;
  QListWidgetItem *item;
  while (query.next()) {
    name = query.value(namendx).toString();
    item =  new QListWidgetItem (bookList);
    item->setText (name);
  }
}

void
FancySearch::PickedTag (QListWidgetItem *item)
{
  RemoveItem (tagList, item);
}

void
FancySearch::PickedBook (QListWidgetItem *item)
{
  RemoveItem (bookList, item);
}

void
FancySearch::RemoveItem (QListWidget *list, QListWidgetItem *item)
{
  if (item && list) {
    int itemRow = list->row (item);
    QListWidgetItem *dead = list->takeItem (itemRow);
    delete dead;
  }
}


} // namespace

