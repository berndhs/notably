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

namespace nota {

ContentMenu::ContentMenu (QWidget *parent)
:QWidget(parent),
 pConf (0),
 pDB (0),
 tagSearch (this)
{
  tagsAction = new QAction (tr("Find by Tag"), this);
  menu.addAction (tagsAction);
  bookAction = new QAction (tr("Select a Book"), this);
  menu.addAction (bookAction);
  searchAction = new QAction (tr("Search..."), this);
  menu.addAction (searchAction);
  allAction = new QAction (tr("Select All Notes"), this);
  menu.addAction (allAction);
  
  connect (tagsAction, SIGNAL (triggered()), this, SLOT (TagSearchExec()));
  connect (allAction, SIGNAL (triggered()), this, SLOT (SelectAllNotes()));
}

void
ContentMenu::SetDB (QSqlDatabase & db)
{
  pDB = &db;
  tagSearch.SetDB (db);
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
    bool ok = refquery.exec (qryStr);
    
    while (refquery.next()) {
      noteid = refquery.value(0).toLongLong();
      results.insert (noteid);
    }
  }
}

void
ContentMenu::SelectAllNotes ()
{
  QString qryStr ("select noteid from 'notes' where 1");
  QSqlQuery allquery (*pDB);
  qint64 noteid;
  noteSet.clear();
  allquery.exec (qryStr);
  while (allquery.next()) {
    noteid = allquery.value(0).toLongLong();
    noteSet.insert (noteid);
  }
  emit Selected (noteSet);
}


} // namespace


