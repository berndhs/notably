#ifndef NOTETAG_EDIT_H
#define NOTETAG_EDIT_H

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
#include "ui_notetags.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QPixmap>
#include <map>

namespace nota {

typedef std::map <QString, QPixmap*> TagPixMapType;

class NoteTagEdit : public QDialog, public Ui_NoteTagDialog {

Q_OBJECT

private:


public:

  NoteTagEdit (QWidget *parent = 0);
  
  void SetDB (QSqlDatabase & thedb) { db = &thedb; }
  
  void ManageTags (const qint64 noteid, 
                   const QString& notename);
 
  void GetTagnames (const qint64 noteid, QStringList & names);  
  
private slots:

  void Done ();
  void PickedAvailTag (QListWidgetItem *item);
  void PickedHaveTag  (QListWidgetItem *item);

  
private:

  void GetAllTags ();
  void ClearPixes ();
  void FillTagList (QListWidget * list, QStringList & tags);
  void CollectNames (QListWidget * list, QStringList & strings);
  
  qint64  noteId;

  QSqlDatabase  *db;
  QStringList   alltags;
  
  
  TagPixMapType   tagPix;
  
};

}

#endif
