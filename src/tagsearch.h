#ifndef TAGSEARCH_H
#define TAGSEARCH_H

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
#include "utility-types.h"
#include "ui_tagsearch.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QPixmap>
#include <QListWidgetItem>
#include <QStringList>

namespace nota {

class TagSearch : public QDialog, public Ui_TagSearchDialog {

Q_OBJECT

public:

  TagSearch (QWidget *parent = 0);
  
  void SetDB (QSqlDatabase & thedb) { pDB = &thedb; }
  
  int Exec ();
  
  QStringList & SelectedTags () { return selectedNames; }
  
public slots:

  void Done ();
  void DeselectTag (QListWidgetItem *item);
  void SelectTag (QListWidgetItem *item);
  
private:

  void GetAllTags ();
  void FillTagList (QListWidget *list, QStringList &tags);
  void ClearPixes ();

  QSqlDatabase    *pDB;
  
  QStringList   alltags;
  QStringList   selectedNames;
  
  TagPixMapType   tagPix;
  
};

} // namespace

#endif
