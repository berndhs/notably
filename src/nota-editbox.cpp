
#include "nota-editbox.h"
#include <QDebug>

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

namespace nota {

EditBox::EditBox (QWidget * parent)
:QTextEdit(parent)
{
}

void
EditBox::dropEvent (QDropEvent * event)
{
  qDebug () << " drop event " << event;
  QTextEdit::dropEvent (event);
}

void
EditBox::paste ()
{
  qDebug () << " paste " ;
  QTextEdit::paste ();
}

void
EditBox::dragEnterEvent (QDragEnterEvent * event)
{
  qDebug () << " drag enter " << event;
  qDebug () << " mime data" << event->mimeData()->formats();
  if (event->mimeData()->hasFormat("text/plain")) {
    event->acceptProposedAction ();
  }
}

}
