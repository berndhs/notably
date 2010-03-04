#include "tag-edit.h"

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
#include <QFileDialog>
#include <QImage>
#include <QDesktopServices>
#include <QBuffer>
#include "delib-debug.h"

namespace nota {

TagEdit::TagEdit (QWidget *parent) 
:QDialog(parent),
 pDB (0),
 statusCol(0),
 nameCol(1),
 descCol(3),
 iconCol(2),
 loading (false),
 maxIconSize (QSize(64,64))
{
  setupUi (this);
  Setup ();
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  connect (deleteButton, SIGNAL (clicked()), this, SLOT (Delete()));
  connect (addButton, SIGNAL (clicked()), this, SLOT (Add()));
  connect (saveButton, SIGNAL (clicked()), this, SLOT (Save()));
  connect (tagTable, SIGNAL (cellActivated(int,int)),
            this, SLOT (PickedCell (int,int)));
  connect (tagTable, SIGNAL (cellDoubleClicked(int,int)),
            this, SLOT (DoubleClick (int,int)));
  connect (tagTable, SIGNAL (cellChanged (int, int)),
            this, SLOT (ChangedCell (int,int)));
}

TagEdit::~TagEdit ()
{
}

void
TagEdit::Setup ()
{
  defaultPix.load (":img/default.png");
  defaultIcon = QIcon(defaultPix);
  tagStatus[Tag_None] = tr("bad status");
  tagStatus[Tag_Old]  = tr("present");
  tagStatus[Tag_New]  = tr("new");
  tagStatus[Tag_Changed] = tr("changed");
  tagStatus[Tag_Delete] = tr("delete");
}

int
TagEdit::Exec ()
{
  GetAllTags ();
  return exec ();
}

void
TagEdit::GetAllTags ()
{
  tagTable->clearContents ();
  tagTable->setRowCount (0);
  loading = true;
  if (pDB) {
    QString tagQry ("select tagname, description, icon from tags where 1");
    QSqlQuery query (*pDB);
    QString   name, desc;
    int       row(0);
    int       nrows = tagTable->rowCount();
    bool ok = query.exec (tagQry);
    qDebug () << " tag query " << tagQry << " ok "<< ok;
    int       namendx = query.record().indexOf("tagname");
    int       descndx = query.record().indexOf("description");
    int       iconndx = query.record().indexOf("icon");
    QTableWidgetItem *statusItem(0), *nameItem(0), *descItem(0), *iconItem(0);
    QByteArray iconBytes;
    QPixmap    pix;
    QPixmap    emptypix;
    QIcon      tagicon;
    while (query.next()) {
      statusItem = new QTableWidgetItem (tagStatus[Tag_Old]);
      SetNoEdit (statusItem);
      name = query.value(namendx).toString();
      nameItem = new QTableWidgetItem (name);
      SetNoEdit (nameItem);
      desc = query.value(descndx).toString();
      descItem = new QTableWidgetItem (desc);
      iconBytes = query.value(iconndx).toByteArray ();
      pix = emptypix;
      pix.loadFromData (iconBytes);
      tagicon = QIcon(pix);
      iconItem = new QTableWidgetItem (tagicon,"");
      SetNoEdit (iconItem);
      iconItem->setData (Qt::UserRole, iconBytes);
      if (row >= nrows) {
        tagTable->insertRow(row);
        nrows = tagTable->rowCount();
      }
      tagTable->setItem (row,statusCol,statusItem);
      tagTable->setItem (row,nameCol,nameItem);
      tagTable->setItem (row,descCol,descItem);
      tagTable->setItem (row,iconCol,iconItem);
      row++;
    }
  }
  loading = false;
}

void
TagEdit::SetNoEdit (QTableWidgetItem *item)
{
  int flags = item->flags();
  flags &= ~(Qt::ItemIsEditable | Qt::ItemIsDropEnabled );
  flags |= Qt::ItemIsSelectable;
  item->setFlags (static_cast<Qt::ItemFlag>(flags));
}

void
TagEdit::PickedCell (int row, int col)
{
  QTableWidgetItem *item(0);
  QTableWidgetItem *statusItem(0);
  item = tagTable->item (row,col);
  statusItem = tagTable->item(row,statusCol);
  if ((item !=0) && (statusItem !=0)) {
    if (statusItem->text() == tagStatus[Tag_Delete]) {
      deleteButton->setText (tr("Un-Delete"));
    } else {
      deleteButton->setText (tr("Delete"));
    }
  }
}

void
TagEdit::DoubleClick (int row, int col)
{
  if (col == iconCol) {
    QTableWidgetItem *item = tagTable->item (row, col);
    if (item) {
      ImageLoader (item);
    }
  }
}

void
TagEdit::ChangedCell (int row, int col)
{
  if (!loading) {
    QTableWidgetItem *item = tagTable->item (row,col);
    if (item) {
      SetTagState (row, Tag_Changed);
    }
  }
}

void
TagEdit::SetTagState (int row, TagState state)
{
  if (state < Tag_None || state > Tag_High) {
    return;
  }
  QTableWidgetItem *statusItem = tagTable->item(row,statusCol);
  if (statusItem) {
    statusItem->setText (tagStatus[state]);
  }
}

TagState
TagEdit::GetTagState (int row)
{
  QTableWidgetItem * stateItem = tagTable->item (row, statusCol);
  if (stateItem) {
    QString stateName = stateItem->text();
    int stateNum;
    for (stateNum = Tag_None; stateNum < Tag_High; stateNum++) {
      if (stateName == tagStatus[stateNum]) {
        return static_cast<TagState>(stateNum);
      }
    }
  }
  return Tag_None;
}


void
TagEdit::Delete ()
{
  QList<QTableWidgetItem*> ditchThese = tagTable->selectedItems();
  QList<QTableWidgetItem*>::iterator it;
  int row;
  TagState oldstate(Tag_None), newstate(Tag_Delete);
  for (it=ditchThese.begin(); it != ditchThese.end(); it++) {
    row = (*it)->row();
    oldstate = GetTagState (row);
    if (oldstate == Tag_Delete) {
      newstate = Tag_New;
    } else {
      newstate = Tag_Delete;
    }
    SetTagState ((*it)->row(), newstate);
  }
}

void
TagEdit::Add ()
{
  int newrow = tagTable->rowCount();
  tagTable->insertRow (newrow);
  tagTable->selectRow (newrow);
  QTableWidgetItem * statusItem = new QTableWidgetItem (tagStatus[Tag_New]);
  QTableWidgetItem * nameItem = new QTableWidgetItem (tr("new tag"));
  QTableWidgetItem * descItem = new QTableWidgetItem (tr("new tag description"));
  QTableWidgetItem * iconItem = new QTableWidgetItem (defaultIcon,tr(""));

  QByteArray pixBytes;
  QBuffer pixbuf (&pixBytes);
  pixbuf.open (QBuffer::WriteOnly);
  defaultPix.save (&pixbuf, "PNG");
  QVariant var;
  var.setValue (pixBytes);
  iconItem->setData (Qt::UserRole, var);
  SetNoEdit (statusItem);
  SetNoEdit (iconItem);
  tagTable->setItem (newrow, statusCol, statusItem);
  tagTable->setItem (newrow, nameCol, nameItem);
  tagTable->setItem (newrow, descCol, descItem);
  tagTable->setItem (newrow, iconCol, iconItem);
}

void
TagEdit::Save ()
{
  /** make two passes:
      (1) delete all deleted tags, AND all their corresponding entries
          in the tagrefs table
      (2) insert all new tags
      This has to be done in this order, in case of name collisions
  */
  int row;
  TagState state;
  for (row = 0; row < tagTable->rowCount(); row++) {
    state = GetTagState (row);
    if (state == Tag_Delete) {
      DeleteFromDB (row);
    }
  }
  for (row = 0; row < tagTable->rowCount(); row++) {
    state = GetTagState (row);
    if (state == Tag_New) {
      AddToDB (row);
    } else if (state == Tag_Changed) {
      AddToDB (row);
    }
  }
  GetAllTags ();
}

void
TagEdit::DeleteFromDB (int row)
{
  QString tagname;
  QTableWidgetItem * nameItem = tagTable->item(row,nameCol);
  QString qryPattern;
  QString qryText;
  QSqlQuery delQuery (*pDB);
  if (nameItem)  {
    tagname = nameItem->text();
    QString qryPattern ("delete from tagrefs where tagname = '%1'");
    qryText = qryPattern.arg(tagname);
    delQuery.exec (qryText);
    qryPattern = QString ("delete from tags where tagname = '%1'");
    qryText = qryPattern.arg(tagname);
    delQuery.exec (qryText);
  }
}

void
TagEdit::AddToDB (int row)
{
  QTableWidgetItem * nameItem = tagTable->item(row, nameCol);
  QTableWidgetItem * descItem = tagTable->item(row, descCol);
  QTableWidgetItem * iconItem = tagTable->item(row, iconCol);
  if (nameItem && descItem) {
    QString name = nameItem->text();
    QString desc = descItem->text();
    QIcon   icon = iconItem->icon();
    QString qryText ("insert or replace into tags (tagname, description, icon) "
                        " values (?,?,?)");
    QSqlQuery insertQuery (*pDB);
    insertQuery.prepare (qryText);
    QVariant nameVal;
    QVariant descVal;
    QVariant iconVal;
    nameVal.setValue (name);
    descVal.setValue (desc);
    QByteArray bytes = iconItem->data(Qt::UserRole).toByteArray();
    iconVal.setValue (bytes);
    insertQuery.bindValue (0,nameVal);
    insertQuery.bindValue (1,descVal);
    insertQuery.bindValue (2,iconVal);
    bool ok = insertQuery.exec ();
  }
}

void
TagEdit::ImageLoader (QTableWidgetItem *item)
{
   if (item == 0) {
     return;
   }
   QString filename = QFileDialog ::getOpenFileName 
                       (this, tr("Select Icon File"),
                          QDesktopServices::storageLocation
                              (QDesktopServices::HomeLocation),
                          tr("Images (*.png *.jpg )"));
   if (filename.length() > 0) {
     QPixmap tagPix (filename);
     if (!tagPix.isNull()) {
       QIcon tagIcon (tagPix);
       item->setIcon (tagIcon);
       QByteArray pixBytes;
       QBuffer iconData (&pixBytes);
       iconData.open (QBuffer::ReadWrite);
       tagPix.save (&iconData, "PNG");
       item->setData (Qt::UserRole, pixBytes);
     }
   }
}

} // namespace


