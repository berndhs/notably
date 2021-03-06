
#include "editmenu.h"
#include <QMessageBox>
#include <QTimer>

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
:QWidget(parent),
 pConf(0)
{
}

void
EditMenu::Init ()
{

  copyAction = new QAction (tr("Copy"), this);
  menu.addAction (copyAction);
  connect (copyAction, SIGNAL (triggered()), this, SLOT (DoCopy()));
  pasteAction = new QAction (tr("Paste"), this);
  menu.addAction (pasteAction);
  connect (pasteAction, SIGNAL (triggered()), this, SLOT (DoPaste()));
  undoAction = new QAction (tr("Undo"), this);
  menu.addAction (undoAction);
  connect (undoAction, SIGNAL (triggered()), this, SLOT(DoUndo()));
  boldAction = new QAction (tr("Bold"),this);
  menu.addAction (boldAction);
  connect (boldAction, SIGNAL (triggered()), this, SLOT (Bold()));
  italicAction = new QAction (tr("Italic"),this);
  menu.addAction (italicAction);
  connect (italicAction, SIGNAL (triggered()), this, SLOT (Italic()));
  underlineAction = new QAction (tr("Underline"),this);
  menu.addAction (underlineAction);
  connect (underlineAction, SIGNAL (triggered()), 
               this, SLOT (Underline()));
  searchAction = new QAction (tr("Find..."), this);
  menu.addAction (searchAction);
  connect (searchAction, SIGNAL (triggered()), this, SLOT (Search()));
}

void
EditMenu::DoCopy ()
{
  emit SigCopy ();
}

void
EditMenu::DoPaste ()
{
  emit SigPaste ();
}

void
EditMenu::DoUndo ()
{
  emit SigUndo ();
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
EditMenu::Search ()
{
  emit SigLocalSearch ();
}

void
EditMenu::Exec (QPoint here)
{
  menu.popup (here);
}

}

  
