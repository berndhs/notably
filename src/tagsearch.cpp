#include "tagsearch.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

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


namespace nota {


TagSearch::TagSearch (QWidget *parent)
:QDialog(parent)
{
  setupUi (this);
  hasTags->setText (tr("Search for Tags"));
  connect (doneButton, SIGNAL (clicked()), this, SLOT (Done()) );
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (selectedTags, SIGNAL (itemActivated (QListWidgetItem *)),
           this, SLOT (DeselectTag (QListWidgetItem *)));
  connect (availableTags, SIGNAL (itemActivated (QListWidgetItem *)),
           this, SLOT (SelectTag (QListWidgetItem *)));
}

int
TagSearch::Exec ()
{
  GetAllTags ();
  selectedTags->clear();
  FillTagList (availableTags, alltags);
  return exec ();
}


void
TagSearch::GetAllTags ()
{
  QString qryStr ("select tagname, icon from tags where 1");
  QSqlQuery query (*pDB);
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
TagSearch::FillTagList (QListWidget *list, QStringList &tags)
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
TagSearch::DeselectTag ( QListWidgetItem * item)
{
  if (item) {
    int itemRow = selectedTags->row (item);
    QListWidgetItem * deadItem = selectedTags->takeItem (itemRow);
    delete deadItem;
  }
}

void
TagSearch::SelectTag ( QListWidgetItem * item)
{
  if (item) {
    QString name = item->text();
    QList <QListWidgetItem*> haveAlready;
    haveAlready = selectedTags->findItems (name,Qt::MatchCaseSensitive );
    if (haveAlready.size() < 1) {
      QListWidgetItem * newTag = new QListWidgetItem (selectedTags);
      newTag->setText (name);
      newTag->setIcon (item->icon());
      selectedTags->addItem (newTag);
    }
  }
}


void
TagSearch::ClearPixes ()
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
TagSearch::Done ()
{
  selectedNames.clear();
  QListWidgetItem * item(0);
  for (int r=0; r<selectedTags->count(); r++) {
    item = selectedTags->item(r);
    if (item) {
      if (! item->isHidden()) {
         selectedNames.append (item->text());
      }
    }
  }
  accept ();
}


} // namespace

