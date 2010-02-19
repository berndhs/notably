#ifndef EDITMENU_H
#define EDITMENU_H
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

enum FontProperty { FP_none=0, FP_bold, FP_italic, FP_underline, FP_max};

class EditMenu : public QWidget {
Q_OBJECT
public:

  EditMenu (QWidget *parent);
  
  void Exec (QPoint here);
  
private slots:

  void Bold ();
  void Italic ();
  void Underline ();
  void ScreenShot ();

signals:

  void SigFontToggle (const FontProperty);
  void SigShootScreen ();

private:

  QMenu    menu;
  QAction  *boldAction;
  QAction  *italicAction;
  QAction  *underlineAction;
  QAction  *shootAction;

};

}

#endif


