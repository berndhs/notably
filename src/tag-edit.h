#ifndef TAG_EDIT_H
#define TAG_EDIT_H


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

#include "deliberate.h"
#include "ui_tageditor.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QTableWidgetItem>

namespace nota {

enum TagState { Tag_None = 0, 
                 Tag_Old, 
                 Tag_New, 
                 Tag_Changed,
                 Tag_Delete, 
                 Tag_High
                };

class TagEdit  : public QDialog, public Ui_TagEditor {
Q_OBJECT

public:

  TagEdit (QWidget * parent);
  ~TagEdit ();
  
  void SetDB (QSqlDatabase & db) { pDB = &db; }
  
  int  Exec ();
  void Setup ();
  
  
public slots:

  void PickedCell (int row, int col);
  void DoubleClick (int row, int col);
  void ChangedCell (int row, int col);
  void Delete ();
  void Add ();
  void Save ();
  void DeleteFromDB (int row);
  void AddToDB (int row);
  
private:

  void GetAllTags ();
  void SetNoEdit (QTableWidgetItem *item);
  void SetTagState (int row, TagState state);
  void ImageLoader (QTableWidgetItem *item);
  
  TagState GetTagState (int row);


  QSqlDatabase  *pDB;
  
  int statusCol;
  int nameCol;
  int descCol;
  int iconCol;
  
  bool loading;
  bool changingStatus;
  
  QSize maxIconSize;
  
  QIcon defaultIcon;
  QPixmap defaultPix;
  
  QString tagStatus[Tag_High];

};

} // namespace


#endif
