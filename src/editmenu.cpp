
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
  shootAction = new QAction (tr("Screenshot"), this);
  menu.addAction (shootAction);
  connect (shootAction, SIGNAL (triggered()), this, SLOT (ScreenShot ()));
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
EditMenu::ScreenShot ()
{
  QMessageBox box;
  box.setText (tr("Click OK, then wait 5 seconds\nScreenshot taken in those 5 seconds"));
  box.addButton (QMessageBox::Cancel);
  box.addButton (QMessageBox::Ok);
  QTimer::singleShot (20000, &box, SLOT (reject()));
  int result = box.exec ();
  qDebug () << " msg box result " << result;
  qDebug () << " ok is " << QMessageBox::Ok;
  if (result == QMessageBox::Ok) {
    emit SigShootScreen ();
  }
}

void
EditMenu::Exec (QPoint here)
{
  menu.popup (here);
}

}

  
