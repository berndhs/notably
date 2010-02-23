
#include "managemenu.h"

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

ManageMenu::ManageMenu (QWidget * parent)
:QWidget(parent),
 fileDialog (this)
{
  fileUI.setupUi (&fileDialog);
  fileNameAction = new QAction (tr("Data Location"),this);
  menu.addAction (fileNameAction);
  connect (fileNameAction, SIGNAL (triggered()), this, SLOT(ChangeFilename()));
  ConnectDialog ();
}

void
ManageMenu::ConnectDialog ()
{
  connect (fileUI.saveButton, SIGNAL(clicked()),
           this, SLOT (Save()));
  connect (fileUI.cancelButton, SIGNAL (clicked()),
           this, SLOT (Cancel()));
}
void
ManageMenu::Exec (QPoint here)
{
 // show ();
  menu.popup (here);
 // hide ();
}

void
ManageMenu::ChangeFilename ()
{
  fileUI.directoryEdit->setText (pConf->Directory());
  fileUI.notesFileEdit->setText (pConf->FileName());
  fileDialog.exec ();
}

void
ManageMenu::Save ()
{
  pConf->SetFileName (fileUI.notesFileEdit->text());
  pConf->SetDirectory (fileUI.directoryEdit->text());
  pConf->Sync ();
  fileDialog.accept ();
  emit SigReload ();
}

void
ManageMenu::Cancel ()
{
  fileDialog.reject ();
}

} // namespace


