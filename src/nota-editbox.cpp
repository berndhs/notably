
#include "nota-editbox.h"
#include <QDebug>
#include <QUrl>
#include <QList>
#include <QDateTime>
#include <QTime>
#include <QDesktopServices>
#include <QDir>

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
:QTextEdit(parent),
 pConf (0)
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
  QTextEdit::paste ();
}

void
EditBox::GrabHtml ()
{
  QTextCursor curse = textCursor();
  QString text = curse.selectedText ();
  curse.removeSelectedText ();
  curse.insertHtml (" " +text+ " ");
}

void
EditBox::GrabLink ()
{
  QTextCursor curse = textCursor();
  QString text = curse.selectedText ();
  curse.removeSelectedText ();
  QString linkpattern (" <a href=\"%1\">%1</a> ");
  QString linkhtml = linkpattern.arg(text);
  curse.insertHtml (linkhtml);
}

void
EditBox::mousePressEvent (QMouseEvent * event)
{
  if (event) {
    QString anch = this->anchorAt (event->pos());
    if (anch.length () > 0) {
      QUrl url (anch);
      if (url.isValid()) {
        QDesktopServices::openUrl (url);
        return;
      }
    }
  } 
  /// don't know what to do with this one, maybe QTextEdit knows
  QTextEdit::mousePressEvent (event);
}

void
EditBox::dragEnterEvent (QDragEnterEvent * event)
{
  qDebug () << " drag enter " << event;
  qDebug () << " mime data" << event->mimeData()->formats();
  if (event->mimeData()->hasImage()) {
    qDebug () << " ---------- we have an image ";
  }
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
  }
}

QString 
EditBox::ImgFilename (QImage & img)
{
  QDateTime now = QDateTime::currentDateTime ();
  quint64 epochsec = now.toTime_t ();
  QTime     nowTime = QTime::currentTime ();
  quint64  msec = nowTime.msec ();
  QString name = QString ("img-") + QString::number(epochsec) 
                 + QString ("-") + QString::number (msec)
                 + QString (".png");
  return name;
}

void
EditBox::InsertImage (QImage & img)
{
  QString imgname = ImgFilename (img);
  if (pConf) {
    QString path = pConf->Directory ();
    imgname.prepend (QDir::separator());
    imgname.prepend (path);
    img.save (imgname);
    QString pattern (" <img src=\"%1\" /> ");
    insertHtml (pattern.arg(imgname));
    return ;
  }
}

void
EditBox::insertFromMimeData ( const QMimeData * source )
{
#if 0
  qDebug () << " inserting from mime data " << source;
  qDebug () << " urls " << source->urls();
  qDebug () << " text " << source->text ();
  qDebug () << " html " << source->html();
  #endif
  if (source->hasImage()) {
    QImage img = qvariant_cast<QImage> (source->imageData());
    InsertImage (img);
    return;
  }
  QTextEdit::insertFromMimeData (source);
}

}
