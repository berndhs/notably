
#include "editmenu.h"

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

EditMenu::EditMenu (QWidget * parent)
:QWidget(parent)
{
  boldAction = new QAction (tr("Bold"),this);
  menu.addAction (boldAction);
  connect (boldAction, SIGNAL (triggered()), this, SLOT (Bold()));
  italicAction = new QAction (tr("Italic"),this);
  menu.addAction (italicAction);
  connect (italicAction, SIGNAL (triggered()), this, SLOT (Italic()));
  underlineAction = new QAction (tr("Underline"),this);
  menu.addAction (underlineAction);
  connect (underlineAction, SIGNAL (triggered()), this, SLOT (Underline()));
}

void
EditMenu::Bold ()
{
  emit SigFontToggle (FP_bold);
}

void
EditMenu::Italic ()
{
  emit SigFontToggle (FP_italic);
}

void
EditMenu::Underline ()
{
  emit SigFontToggle (FP_underline);
}

void
EditMenu::Exec (QPoint here)
{
  menu.popup (here);
}

}

  
