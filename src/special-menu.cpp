#include "special-menu.h"

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
#include <QMessageBox>
#include <QTimer>

using namespace deliberate;

namespace nota {


SpecialMenu::SpecialMenu (QWidget *parent)
:QWidget(parent),
 pConf (0)
{
}

void
SpecialMenu::SetConf (NotaConf * pC)
{
  pConf = pC;
}

void
SpecialMenu::Init ()
{
  if (pConf) {
    if (pConf->SupportPartScreenShot()) {
      shootAction = new QAction (tr("Part Screen Shot"), this);
      menu.addAction (shootAction);
      connect (shootAction, SIGNAL (triggered()), this, SLOT (ScreenShot ()));
    }
  }
  shootAllAction = new QAction (tr("Whole Screen Shot"), this);
  menu.addAction (shootAllAction);
  if (!IsFingerInterface()) {
    menu.addSeparator ();
  }
  connect (shootAllAction, SIGNAL (triggered()), this, SLOT (WholeScreenShot()));
  htmlAction = new QAction (tr("Insert Selection as Html"), this);
  menu.addAction (htmlAction);
  connect (htmlAction, SIGNAL (triggered()), this, SLOT (GrabHtml()));
  linkAction = new QAction (tr("Make Selection a Link"), this);
  menu.addAction (linkAction);
  connect (linkAction, SIGNAL (triggered ()), this, SLOT (GrabLink()));
  if (!IsFingerInterface()) {
    menu.addSeparator ();
  }
  copyLinkAction = new QAction (tr("Copy Inter-Note Link"), this);
  menu.addAction (copyLinkAction);
  connect (copyLinkAction, SIGNAL (triggered ()), this, SLOT (GrabNoteLink()));
  pasteLinkAction = new QAction (tr("Paste Inter-Note Link"), this);
  menu.addAction (pasteLinkAction);
  connect (pasteLinkAction, SIGNAL (triggered()), this, SLOT (PasteNoteLink()));
}

void
SpecialMenu::EmitScreenShot (const QString msg, const bool whole)
{
  QMessageBox box(this);
  Qt::WindowFlags flags = box.windowFlags();
  flags |= Qt::FramelessWindowHint;
  box.setWindowFlags (flags);
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
SpecialMenu::ScreenShot ()
{
  EmitScreenShot (
  tr("Click OK, then wait 5 seconds\n"
     "Grab Screen Section to Capture\n"
     "Screenshot taken in those 5 seconds"),
                  false);
}

void
SpecialMenu::WholeScreenShot ()
{
  EmitScreenShot (
  tr("Click OK, then wait 5 seconds\n"
      "Notably window will hide for 5 seconds\n"
      "Entire Screen captured in those 5 seconds")
      , true  );
}


void
SpecialMenu::GrabLink ()
{
  emit SigGrabLink ();
}

void
SpecialMenu::GrabHtml ()
{
  emit SigGrabSelection ();
}

void
SpecialMenu::GrabNoteLink ()
{
  emit SigCopyNoteLink ();
}

void
SpecialMenu::PasteNoteLink ()
{
  emit SigPasteNoteLink ();
}


void
SpecialMenu::Exec (QPoint here)
{
  menu.popup (here);
}

} // namespace
