
#include "nota-editbox.h"
#include <QDebug>
#include <QUrl>
#include <QList>
#include <QDateTime>
#include <QTime>
#include <QDesktopServices>
#include <QSqlQuery>
#include <QSqlResult>
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
 pConf (0),
 pDB(0),
 searchBox (this)
{
  SetupSearchbox ();
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

QString
EditBox::FindUsergivenId (qint64 noteid)
{
  QString result ("");
  qDebug () << " look for name for " << noteid;
  if (pDB) {
    QString qryPattern ("select usergivenid from notes where noteid=%1");
    QString qryStr = qryPattern.arg(QString::number(noteid));
    QSqlQuery query (*pDB);
    bool ok = query.exec (qryStr);
    qDebug () << " query " << qryStr << " was " << ok;
    if (ok && query.next()) {
      result = query.value(0).toString();
    }
  }
  return result;
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
  QString linkpattern (" <a href=\"%1\">%2</a> ");
  QUrl url(text);
  QString linkhtml;
  qDebug () << " grabbed url " << url;
  qDebug () << " scheme " << url.scheme();
  if (url.isValid() && url.scheme() == "ntbly") {
    QString longname = FindUsergivenId (url.authority().toLongLong());
    if (longname == QString("")) {
      longname = text;
    }
    linkhtml = linkpattern.arg(text).arg(longname);
  } else {
    linkhtml = linkpattern.arg(text).arg(text);
  }
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
        if (url.scheme() == "ntbly") {
          qint64 newnote = url.authority().toLongLong();
          emit LinkToNote (newnote);
        } else {
          QDesktopServices::openUrl (url);
        }
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
    QString path = pConf->ImagePath ();
    imgname.prepend (QDir::separator());
    imgname.prepend (path);
    QString completePath = pConf->CompleteImagePath ();
    QDir imgdir (completePath);
    if (!imgdir.exists()) {
      imgdir.mkpath (completePath);
    }
    img.save (imgname);
    QString pattern (" <img src=\"%1\" /> ");
    insertHtml (pattern.arg(imgname));
    emit NewImage (imgname);
    return ;
  }
}

void
EditBox::insertFromMimeData ( const QMimeData * source )
{
  if (source->hasImage()) {
    QImage img = qvariant_cast<QImage> (source->imageData());
    InsertImage (img);
    return;
  }
  QTextEdit::insertFromMimeData (source);
}


void
EditBox::SetupSearchbox ()
{
  searchUi.setupUi (&searchBox);
  searchBox.hide();
  connect (searchUi.searchButton, SIGNAL (clicked()), 
           this, SLOT (DoSearch()));
  connect (searchUi.cancelButton, SIGNAL (clicked()),
           &searchBox, SLOT (reject()));
}

void
EditBox::DoSearch ()
{
  QString lookforthis = searchUi.searchWord->text();
  find (lookforthis);
  if (deliberate::IsMaemo()) {
    searchBox.accept ();
  }
}

void
EditBox::LocalSearch ()
{
  //searchBox.show();
  searchBox.exec ();
}

} // namespace

