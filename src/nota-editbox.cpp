
#include "nota-editbox.h"
#include <QDebug>
#include <QUrl>
#include <QList>

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
  qDebug () << " editbox drop event " << event;
  if (event->mimeData()->hasUrls()) {
    qDebug() << "URLs: " << event->mimeData()->urls();
    IsolateUrls (event);
  } else {
    qDebug () << " we don't have any urls";
    QTextEdit::dropEvent (event);
  }
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
  if (event->mimeData()->hasUrls()) {
    event->acceptProposedAction ();
  } else  if (event->mimeData()->hasFormat("text/plain")) {
    event->acceptProposedAction ();
  }
}

void
EditBox::IsolateUrls (QDropEvent * event)
{
  QList<QUrl> urlList = event->mimeData()->urls();
  QList<QUrl>::iterator uit;
  QString ustr;
  QString anchor (" <a href=\"%1\">%2</a> ");
  QString insert;
  for (uit=urlList.begin(); uit != urlList.end(); uit++) {
    ustr = uit->toString ();
    insert = anchor.arg(ustr).arg(ustr);
    insertHtml (insert);
    qDebug () << " inserted " << insert;
  }
  qDebug () << " text with urls: " << event->mimeData()->text();
  qDebug () << " html with urls: " << event->mimeData()->html();
  qDebug () << " mime data: " << event->mimeData();
}

void
EditBox::insertFromMimeData ( const QMimeData * source )
{
  qDebug () << " inserting from mime data " << source;
  qDebug () << " urls " << source->urls();
  qDebug () << " text " << source->text ();
  qDebug () << " html " << source->html();
  QTextEdit::insertFromMimeData (source);
}

}
