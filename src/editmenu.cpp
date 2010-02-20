
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
  if (pConf) {
    if (pConf->SupportPartScreenShot()) {
      shootAction = new QAction (tr("Part Screen Shot"), this);
      menu.addAction (shootAction);
      connect (shootAction, SIGNAL (triggered()), this, SLOT (ScreenShot ()));
    }
  }
  shootAllAction = new QAction (tr("Whole Screen Shot"), this);
  menu.addAction (shootAllAction);
  connect (shootAllAction, SIGNAL (triggered()), this, SLOT (WholeScreenShot()));
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
EditMenu::EmitScreenShot (const QString msg, const bool whole)
{
  QMessageBox box;
  box.setText (msg);
  box.addButton (QMessageBox::Cancel);
  box.addButton (QMessageBox::Ok);
  QTimer::singleShot (20000, &box, SLOT (reject()));
  int result = box.exec ();
  if (result == QMessageBox::Ok) {
    emit SigShootScreen (whole);
  }
}

void
EditMenu::ScreenShot ()
{
  EmitScreenShot (
  tr("Click OK, then wait 5 seconds\n"
     "Grab Screen Section to Capture\n"
     "Screenshot taken in those 5 seconds"),
                  false);
}

void
EditMenu::WholeScreenShot ()
{
  EmitScreenShot (
  tr("Click OK, then wait 5 seconds\n"
      "Notably window will hide for 5 seconds\n"
      "Entire Screen captured in those 5 seconds")
      , true  );
}

void
EditMenu::Exec (QPoint here)
{
  menu.popup (here);
}

}

  
