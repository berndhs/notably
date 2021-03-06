
#include "notemenu.h"
#include "deliberate.h"

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

using namespace deliberate;

namespace nota {

NoteMenu::NoteMenu (QWidget * parent)
:QWidget(parent)
{
  saveAction = new QAction (tr("Save Note"),this);
  deleteAction = new QAction (tr("Delete Note"), this);
  newAction = new QAction (tr("New Note"), this);
  cancelAction = new QAction (tr("Cancel Note"), this);
  tagsAction = new QAction (tr("Note Tags..."), this);
  booksAction = new QAction (tr("Note Books..."), this);
  publishAction = new QAction (tr("Publish Note..."), this);
  menu.addAction (saveAction);
  if (!IsFingerInterface ()) {
    menu.addSeparator();
  }
  menu.addAction (deleteAction);
  menu.addAction (newAction);
  menu.addAction (cancelAction);
  if (!IsFingerInterface()) {
    menu.addSeparator();
  }
  menu.addAction (tagsAction);
  menu.addAction (booksAction);
  menu.addAction (publishAction);
  connect (saveAction, SIGNAL (triggered()), this, SLOT(SaveCurrent()));
  connect (tagsAction, SIGNAL (triggered()), this, SLOT (Tags()));
  connect (booksAction, SIGNAL (triggered()), this, SLOT (Books()));
  connect (publishAction, SIGNAL (triggered()), this, SLOT (PublishCurrent()));
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
NoteMenu::Tags ()
{
  emit ChangeTags ();
}

void
NoteMenu::Books ()
{
  emit ChangeBooks ();
}

void
NoteMenu::PublishCurrent ()
{
  emit PublishNote ();
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

