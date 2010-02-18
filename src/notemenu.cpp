
#include "notemenu.h"

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

NoteMenu::NoteMenu (QWidget * parent)
:QWidget(parent)
{
  saveAction = new QAction (tr("Save Note"),this);
  deleteAction = new QAction (tr("Delete Note"), this);
  newAction = new QAction (tr("New Note"), this);
  cancelAction = new QAction (tr("Cancel Note"), this);
  menu.addAction (saveAction);
  menu.addAction (deleteAction);
  menu.addAction (newAction);
  menu.addAction (cancelAction);
  connect (saveAction, SIGNAL (triggered()), this, SLOT(SaveCurrent()));
  connect (deleteAction, SIGNAL (triggered()), this, SLOT(DeleteCurrent()));
  connect (newAction, SIGNAL (triggered()), this, SLOT(MakeNewNote()));
  connect (cancelAction, SIGNAL (triggered()), this, SLOT (CancelCurrent()));
}

void
NoteMenu::Exec (QPoint here)
{
 // show ();
  menu.popup (here);
 // hide ();
}

void
NoteMenu::SaveCurrent ()
{
  emit SaveNote();
}

void
NoteMenu::DeleteCurrent ()
{
  emit DeleteNote ();
}

void
NoteMenu::MakeNewNote ()
{
  emit NewNote ();
}

void
NoteMenu::CancelCurrent ()
{
  emit CancelNote ();
}

}

