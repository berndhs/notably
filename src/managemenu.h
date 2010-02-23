#ifndef MANAGEMENU_H
#define MANAGEMENU_H
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

#include "deliberate.h"
#include <QWidget>
#include <QMenu>
#include "delib-debug.h"
#include "notaconf.h"
#include "ui_changefile.h"

namespace nota {

class ManageMenu : public QWidget {
Q_OBJECT

public:

  ManageMenu (QWidget *parent);
  
  void SetConf (NotaConf * pC) { pConf = pC; }
  
  void Exec (QPoint here);
  
public slots:

  void ChangeFilename ();
  void Save ();
  void Cancel ();
  
signals:

  void SigReload ();

private:

  void ConnectDialog ();

  QMenu   menu;
  Ui_ChangeFile   fileUI;
  QDialog         fileDialog;
  
  NotaConf  *pConf;
  
  QAction  *fileNameAction;
  
};

}


#endif

