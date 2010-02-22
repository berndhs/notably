#ifndef NOTEMENU_H
#define NOTEMENU_H
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

#include <QWidget>
#include <QMenu>
#include "delib-debug.h"

namespace nota {

class NoteMenu : public QWidget {
Q_OBJECT

public:

  NoteMenu (QWidget *parent);
  
  void Exec (QPoint here);
  
public slots:

  void SaveCurrent ();
  void PublishCurrent ();
  void DeleteCurrent ();
  void MakeNewNote ();
  void CancelCurrent ();
  void Tags ();
  
signals:

  void SaveNote ();
  void PublishNote ();
  void DeleteNote ();
  void NewNote ();
  void CancelNote ();
  void ChangeTags ();
  
private:

  QMenu   menu;
  
  QAction  *saveAction;
  QAction  *publishAction;
  QAction  *deleteAction;
  QAction  *newAction;
  QAction  *cancelAction;
  QAction  *tagsAction;
  
};

}


#endif

