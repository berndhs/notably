#include "notetag-edit.h"

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

NoteTagEdit::NoteTagEdit (QWidget *parent)
:QDialog (parent),
 db(0)
{
  setupUi (this);
  connect (doneButton, SIGNAL (clicked()), this, SLOT (Done()) );
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (noteTags, SIGNAL (itemActivated (QListWidgetItem *)),
           this, SLOT (PickedHaveTag (QListWidgetItem *)));
  connect (availableTags, SIGNAL (itemActivated (QListWidgetItem *)),
           this, SLOT (PickedAvailTag (QListWidgetItem *)));
}

void
NoteTagEdit::ManageTags (const qint64 noteid,
                         const QString & notename)
{
  if (db == 0) {
    return;
  }
  noteId = noteid;
  QStringList tagnames;
  GetAllTags ();
  noteName->setText (notename);
  GetTagnames (noteid, tagnames);
  FillTagList (noteTags, tagnames);
  FillTagList (availableTags, alltags);
  exec ();
}

void
NoteTagEdit::GetTagnames ( const qint64 noteid, QStringList & names)
{
  names.clear ();
  if (db == 0) {
    return;
  }
  QString tagsPattern ("select tagname from tagrefs where noteid = %1");
  QString tagsQuery = tagsPattern.arg(QString::number(noteid));
  QSqlQuery query (*db);
  query.exec (tagsQuery);
  int tagField = query.record().indexOf ("tagname");
  QString tagname;
  while (query.next()) {
    names.append (query.value(tagField).toString());
  }
}

void
NoteTagEdit::PickedHaveTag ( QListWidgetItem * item)
{
  if (item) {
    int itemRow = noteTags->row (item);
    QListWidgetItem * deadItem = noteTags->takeItem (itemRow);
    delete deadItem;
  }
}

void
NoteTagEdit::PickedAvailTag ( QListWidgetItem * item)
{
  if (item) {
    QString name = item->text();
    QList <QListWidgetItem*> haveAlready;
    haveAlready = noteTags->findItems (name,Qt::MatchCaseSensitive );
    if (haveAlready.size() < 1) {
      QListWidgetItem * newTag = new QListWidgetItem (noteTags);
      newTag->setText (name);
      newTag->setIcon (item->icon());
      noteTags->addItem (newTag);
    }
  }
}


void
NoteTagEdit::GetAllTags ()
{
  QString qryStr ("select tagname, icon from tags where 1");
  QSqlQuery query (*db);
  query.exec (qryStr);
  int nameindex = query.record().indexOf ("tagname");
  int iconindex = query.record().indexOf ("icon");
  QString name;
  QPixmap *pix;
  QByteArray pixBytes;
  ClearPixes ();
  alltags.clear ();
  while (query.next () ) {
    name = query.value (nameindex).toString ();
    pixBytes = query.value (iconindex).toByteArray ();
    pix = new QPixmap;
    pix->loadFromData (pixBytes);
    tagPix[name] = pix;
    alltags.append (name);
  }
}

void
NoteTagEdit::ClearPixes ()
{
  TagPixMapType::iterator pit;
  for (pit = tagPix.begin(); pit != tagPix.end(); pit++) {
    if (pit->second) {
      delete pit->second;
      pit->second = 0;
    }
  }
  tagPix.clear ();
}

void
NoteTagEdit::FillTagList (QListWidget *list, QStringList &tags)
{
  QStringList::iterator tagit;
  QListWidgetItem      *item(0);
  QString               name;
  
  list->clear ();
  for (tagit = tags.begin(); tagit != tags.end(); tagit++) {
    item = new QListWidgetItem (list);
    name = *tagit;
    item->setText (name);
    if (tagPix.count (name) > 0) {
      item->setIcon (QIcon (*(tagPix[name])))  ;
    }
  }
}

void
NoteTagEdit::CollectNames (QListWidget *list, QStringList & strings)
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
NoteTagEdit::Done ()
{
  QString deletePattern ("delete from tagrefs where noteid = %1");
  QString deleteStr = deletePattern.arg(noteId);
  QSqlQuery query (*db);
  query.exec (deleteStr);
  QStringList haveTags;
  CollectNames (noteTags, haveTags);
  QString addPattern ("insert or replace into tagrefs ( noteid, tagname )"
                      " values (%1,'%2')");
  QString addStr;
  QStringList::iterator tagit;
  for (tagit = haveTags.begin(); tagit != haveTags.end(); tagit++) {
    addStr = addPattern.arg(noteId).arg (*tagit);
    query.exec (addStr);
  }
  accept ();
}


} // namespace

