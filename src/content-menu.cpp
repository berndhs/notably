#include "content-menu.h"


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
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QTextEdit>
#include <QMessageBox>

namespace nota {

ContentMenu::ContentMenu (QWidget *parent)
:QWidget(parent),
 pConf (0),
 pDB (0),
 tagSearch (this),
 bookPick (this),
 searchBox (this)
{

  SetupSearchbox ();
  
  tagsAction = new QAction (tr("Find by Tag"), this);
  menu.addAction (tagsAction);
  notagAction = new QAction (tr("Select untagged Notes"), this);
  menu.addAction (notagAction);
  bookAction = new QAction (tr("Select by Book"), this);
  menu.addAction (bookAction);
  nobookAction = new QAction (tr("Select unbooked Notes"), this);
  menu.addAction (nobookAction);
  searchAction = new QAction (tr("Search..."), this);
  menu.addAction (searchAction);
  allAction = new QAction (tr("Select All Notes"), this);
  menu.addAction (allAction);
  
  connect (tagsAction, SIGNAL (triggered()), this, SLOT (TagSearchExec()));
  connect (notagAction, SIGNAL (triggered()), this, SLOT (NotagNotes()));
  connect (searchAction, SIGNAL (triggered()), this, SLOT (MultiSearch()));
  connect (bookAction, SIGNAL (triggered()), this, SLOT (Books ()));
  connect (nobookAction, SIGNAL (triggered()), this, SLOT (NobookNotes()));
  connect (allAction, SIGNAL (triggered()), this, SLOT (SelectAllNotes()));
}

void
ContentMenu::SetDB (QSqlDatabase & db)
{
  pDB = &db;
  tagSearch.SetDB (db);
  bookPick.SetDB (db);
}

NoteIdSetType &
ContentMenu::SelectedNotes ()
{
  return noteSet;
}

void
ContentMenu::Exec (QPoint here)
{
  menu.popup (here);
}

void
ContentMenu::TagSearchExec ()
{
  int theywantit = tagSearch.Exec();
  if (theywantit) {
    FindSelectedNotes (noteSet, tagSearch.SelectedTags());
    emit Selected (noteSet);
    //emit DoneSelection ();
  } else {
    noteSet.clear ();
  }
}

void
ContentMenu::Books ()
{
  bookPick.Exec ();
  if (bookPick.SelectedBook()) {
    QString qryPattern 
      ("select distinct noteid from bookrefs where bookname ='%1'");
    QString qryStr = qryPattern.arg(bookPick.TitleSelected());
    SingleIdQuery (noteSet, qryStr);
    emit Selected (noteSet);
  }
}

void
ContentMenu::FindSelectedNotes (NoteIdSetType & results,
                                QStringList & tagnames)
{
  QString qryPattern ("select noteid from 'tagrefs' where tagname ='%1'");
  QStringList::iterator tagit;
  QSqlQuery refquery (*pDB);
  qint64  noteid;
  results.clear();
  for (tagit = tagnames.begin(); tagit != tagnames.end(); tagit++) {
    QString qryStr = qryPattern.arg (*tagit);
    refquery.exec (qryStr);
    while (refquery.next()) {
      noteid = refquery.value(0).toLongLong();
      results.insert (noteid);
    }
  }
}

void
ContentMenu::SelectAllNotes (bool doemit)
{
  QString qryStr ("select noteid from 'notes' where 1");
  SingleIdQuery (noteSet, qryStr);
  if (doemit ) {
    emit Selected (noteSet);
  }
}

void
ContentMenu::NotagNotes ()
{
  QString qryStr ("select noteid from notes where 1"
                     " except select distinct noteid from bookrefs where 1");
  SingleIdQuery (noteSet, qryStr);
  emit Selected (noteSet);
}

void
ContentMenu::NobookNotes ()
{
  QString qryStr ("select noteid from notes where 1"
                     " except select distinct noteid from bookrefs where 1");
  SingleIdQuery (noteSet, qryStr);
  emit Selected (noteSet);
}

void
ContentMenu::SingleIdQuery (NoteIdSetType & theNoteSet, QString qryStr)
{
  theNoteSet.clear();
  QSqlQuery query (*pDB);
  query.exec (qryStr);
  qint64 noteid;
  while (query.next()) {
    noteid = query.value(0).toLongLong();
    theNoteSet.insert (noteid);
  }
}


void
ContentMenu::SetupSearchbox ()
{
  searchUi.setupUi (&searchBox);
  searchBox.setWindowTitle (tr("Notes with ALL these words"));
  searchBox.hide();
  connect (searchUi.searchButton, SIGNAL (clicked()), 
           this, SLOT (DoSearch()));
  connect (searchUi.cancelButton, SIGNAL (clicked()),
           &searchBox, SLOT (reject()));
}


void
ContentMenu::MultiSearch ()
{
  searchBox.exec ();
}

void
ContentMenu::DoSearch ()
{
  QString findthis = searchUi.searchWord->text();
  QStringList findWords = findthis.split (" ", QString::SkipEmptyParts);
  if (findWords.count () < 1) {
    searchBox.reject();
    return;
  }
  NoteIdSetType  tmpSet;
  QString allNotes ("select noteid, notetext from notes where 1");
  QSqlQuery query (*pDB);
  bool ok = query.exec (allNotes);
  int  idndx = query.record().indexOf ("noteid");
  int  textndx = query.record().indexOf ("notetext");
  QTextEdit  editBuf;
  editBuf.setHidden (true);
  int      hits (0);
  QString  text;
  qint64   id;
  bool     isthere(false);
  QString word;
  QStringList::iterator fit;
  while (ok && query.next ()) {
    id = query.value(idndx).toLongLong ();
    text = query.value(textndx).toString ();
    editBuf.setHtml (text);
    isthere = true;
    for (fit = findWords.begin(); fit != findWords.end(); fit++) {
      word = *fit;
      isthere &= editBuf.find (word);
      if (!isthere) {
        break;
      }
    }
    if (isthere) {
      tmpSet.insert (id);
      hits++;
    }
  }
  if (hits > 0) {
    noteSet = tmpSet;
    searchBox.accept();
    emit Selected (noteSet);
  } else {
    QMessageBox box (this);
    box.setText (tr("No Matching Notes"));
    Qt::WindowFlags flags = box.windowFlags();
    flags |= Qt::FramelessWindowHint;
    box.setWindowFlags (flags);
    box.exec ();
  }
}


} // namespace


