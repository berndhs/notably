
#include "export-html.h"

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
#include <QDebug>

#include <QTimer>

namespace nota {

ExportHtml::ExportHtml (QWidget *parent, QSqlDatabase &db)
:QWidget(parent)
{
  pDB = &db;
}

bool
ExportHtml::IsImplemented ()
{
  #if DELIBERATE_HAVE_WEBELT
  return true;
  #else
  return false;
  #endif
}

bool
ExportHtml::ExportBook (QString bookname)
{
   #if DELIBERATE_HAVE_WEBELT
   return PrivateExportBook (bookname);
   #else
   return false;
   #endif
}

bool
ExportHtml::ExportNote (qint64 id, QString copyToHere, QString oldPath)
{
  QFile pageFile (copyToHere);
  pageFile.open (QFile::WriteOnly);
  QByteArray  noteView;
  ReadNote (id, noteView);
  qint64 nbytes = pageFile.write (noteView);
  pageFile.close();
  QFileInfo destInfo (copyToHere);
  QMessageBox report (this);
  Qt::WindowFlags flags = report.windowFlags();
  flags |= Qt::FramelessWindowHint;
  report.setWindowFlags (flags);
  QString savedMessage;
  if (nbytes > 0) {
    savedMessage = QString (tr("Note exported as %1"))
                            .arg (copyToHere);  
    if (oldPath != destInfo.path()) {
      CopyPageImages (id, oldPath, destInfo.path ());
    }
  } else {
    savedMessage = QString (tr("Could not write %1 !"))
                            .arg (copyToHere);
  }
  report.setText (savedMessage);
  QTimer::singleShot (20000, &report, SLOT(accept()));
  report.exec ();
  return true;
}

void
ExportHtml::ExportAllImages (const QString srcpath, const QString destpath)
{
  QSet<qint64> idSet;
  QString      allnotes ("select distinct noteid from imagerefs where 1");
  QSqlQuery    query(*pDB);
  qint64       id;
  query.exec (allnotes);
  while (query.next()) {
    id = query.value(0).toLongLong();
    idSet.insert (id);
  }
  QSet<qint64>::iterator nit;
  for (nit = idSet.begin(); nit != idSet.end(); nit++) {
    id = *nit;
    CopyPageImages (id, srcpath, destpath);
  }
}

void
ExportHtml::CopyPageImages (const qint64 noteid,
                              const QString srcdir,
                              const QString destdir)
{

  QString selectPattern ("select imageref from imagerefs where noteid = %1");
  QString select = selectPattern.arg(QString::number(noteid));
  QSqlQuery query (*pDB);
  QString imgname;
  QString srcfile;
  QString destfile;
  QChar    sep = QDir::separator();
  bool ok = query.exec (select);
  QString imgdir;
  while (query.next()) {
    imgname = query.value(0).toString();
    srcfile = srcdir + sep + imgname;
    destfile = destdir + sep + imgname;
    QFileInfo imginfo (destfile);
    imgdir = imginfo.path();
    QDir dest (imgdir);
    if (!dest.exists()) {
      dest.mkpath (imgdir);
    }
    QFile oldFile (srcfile);
    QFile newFile (destfile);
    if (newFile.exists()) {
      newFile.remove();
    }
    ok = oldFile.copy (destfile);
  }
  
}

void
ExportHtml::ReadNote (qint64 id, QByteArray & noteView)
{
  QString pat ("select notetext from notes where noteid=%1");
  QString qryStr = pat.arg(QString::number(id));
  QSqlQuery query (*pDB);
  bool ok = query.exec (qryStr);
  if (ok && query.next()) {
    noteView = query.value(0).toByteArray();
  } else {
    noteView.clear();
  }
}

#if DELIBERATE_HAVE_WEBELT

bool
ExportHtml::PrivateExportBook (QString bookname)
{
  pageTitle = bookname;
  QString saveFile = 
           QFileDialog::getSaveFileName 
                      (this, "File Name for Index page",
            QDesktopServices::storageLocation (QDesktopServices::HomeLocation)
            + QDir::separator() + pageTitle + "_index.html",
            QObject::tr ("Web pages (*.html);;All Files (*.*)"));
  if (saveFile.length() == 0) {
    return false; // nevermind
  }
  exportSet.clear();
  exportNames.clear();
  imageCounter = 0;
  imageDir = pageTitle + "_images";
  MakePaths (saveFile);
  QSet<qint64>  initialNotes;
  GetBookNotes (bookname, initialNotes);
  qint64 noteid;
  QSet<qint64>::iterator nit;
  for (nit = initialNotes.begin(); nit != initialNotes.end(); nit++) {
    noteid = *nit;
    exportSet.insert (noteid);
    exportNames [noteid] = GetNoteTitle(noteid);
    FillNoteIdSet (noteid);
  }
  ConstructPages (saveFile, exportSet, exportNames);
 
  QUrl newurl (saveFile);
  QDesktopServices::openUrl (newurl);
  return true;
}

void
ExportHtml::GetBookNotes (QString bookname, QSet<qint64> & idset)
{
  QString pat ("select distinct noteid from bookrefs where bookname='%1'");
  QString qryStr = pat.arg(bookname);
  QSqlQuery query (*pDB);
  bool ok = query.exec (qryStr);
  qint64 id;
  while (ok && query.next()) {
    id = query.value(0).toLongLong();
    idset.insert (id);
  }
}

QString 
ExportHtml::GetNoteTitle (qint64 noteId)
{
   QString pat ("select usergivenid from notes where noteid =%1");
   QString qryStr = pat.arg(noteId);
   QSqlQuery query (*pDB);
   QString name("");
   bool ok = query.exec (qryStr);
   if (ok && query.next()) {
     name = query.value(0).toString();
   } 
   return name;
}


void
ExportHtml::FillNoteIdSet (qint64 startId)
{
  qint64 id = startId;
  QSqlQuery query (*pDB);
  QString pat ("select notetext from notes where noteid=%1");
  QString qstr = pat.arg(QString::number(id));
  query.exec (qstr);
  QString fulltext;
  if (query.next()) {
    fulltext = query.value(0).toString();
    QWebView view;
    view.setHtml (fulltext);
    QWebPage *pg = view.page();
    QWebElementCollection allElements = pg->mainFrame()->findAllElements("*");
    QString tag;
    QString ref;
    foreach (QWebElement elt, allElements) {
      tag = elt.tagName().toUpper();
      if (tag == "A") {
         QUrl url (elt.attribute("href"));
         QString sch = url.scheme();
         if (sch == "notably") {
           qint64 newid = url.authority().toLongLong();
           if (newid != 0) {
             if (!exportSet.contains (newid)) {
               exportSet.insert (newid);
               exportNames [newid] = GetNoteTitle (newid);
               FillNoteIdSet (newid);
             }
           }
         }
      } else if (tag == "IMG") {
        QUrl url (elt.attribute("src"));
        TranslateImageLoc (url);
      }     
    }
  }
}

void
ExportHtml::ConstructPages (QString indexname, 
                          QSet<qint64> &idSet,
                          std::map<qint64,QString> & nameTable)
{
  QFileInfo info (indexname);
  ConstructIndexpage (pageTitle, idSet, nameTable, indexname);
  if (imgNameTranslate.size() > 0 ) {
    MakeImageDir ();
    CopyImages ();
  }
  QSet<qint64>::iterator pg;
  for (pg = idSet.begin(); pg != idSet.end(); pg++) {
    ConstructWebpage (*pg, info.path());
  }
}

QString
ExportHtml::LinkFileName (qint64 id)
{
  QString pat ("Note%1.html");
  return pat.arg(QString::number(id));
}

void
ExportHtml::ConstructIndexpage (QString pagename,
                               QSet<qint64> & idSet,
                               std::map<qint64,QString> & nameTable,
                               QString completeName)
{
  QString pagePat ("<html><head><title>%1</title></head>\n"
           "<body>\n<h1>%2</h1>\n%3\n</body>\n</html>\n");
  QString linkPat ("<a href=\"%1\">%2</a><br>\n");
  QSet<qint64>::iterator nit;
  QString body;
  for (nit = idSet.begin(); nit != idSet.end(); nit++) {
    body.append (linkPat.arg(LinkFileName(*nit)).arg(nameTable[*nit]));
  }
  QString page = pagePat.arg(pagename).arg(pagename).arg(body);
  QTextEdit htmlPage;
  htmlPage.setVisible (false);
  htmlPage.setHtml (page);
  QFile dest (completeName);
  dest.open (QFile::WriteOnly);
  dest.write (htmlPage.toHtml().toLocal8Bit());
  dest.close ();
  
}

void
ExportHtml::ConstructWebpage (qint64 noteid, QString path)
{
  QString pat ("select notetext from notes where noteid=%1");
  QString qryStr = pat.arg(QString::number(noteid));
  QSqlQuery query (*pDB);
  bool ok = query.exec (qryStr);
  if (ok && query.next()) {
    QWebView view;
    view.setHtml (query.value(0).toString());
    QWebPage *pg = view.page();
    QWebElementCollection allElements = pg->mainFrame()->findAllElements("*");
    QString tag;
    QString ref;
    foreach (QWebElement elt, allElements) {
      tag = elt.tagName().toUpper();
      if (tag == "A") {
        ref = elt.attribute ("href");
        QUrl url(ref);
        QString sch = url.scheme();
        if (sch == "notably") {
          qint64 target = url.authority().toLongLong();
          if (target != 0) {
            elt.setAttribute ("href",LinkFileName(target));
          }
        }
      } else if (tag == "IMG") {
        ref = elt.attribute ("src");
        if (imgNameTranslate.count(ref) > 0) {
          elt.setAttribute ("src",imgNameTranslate[ref].newFile);
        }
      }
    }
    QString newpage = pg->mainFrame()->toHtml();
    QString filename = LinkFileName (noteid);
    QFile dest (path + QDir::separator() + filename);
    dest.open (QFile::WriteOnly);
    dest.write (newpage.toLocal8Bit());
    dest.close ();
    
  }
}

void
ExportHtml::TranslateImageLoc (QUrl url)
{
  QString whole = url.toString();
  if (imgNameTranslate.count(whole) > 0) { // seen it, use old translation
    return;
  }
  if (url.scheme().isEmpty()) {
    QString newref (NewImagename(whole));
    imgNameTranslate[whole] = ImageNameTranslate (whole, newref);
  } else {
    // it is a real URL, not a file name, so we just keep it
    imgNameTranslate[whole] = ImageNameTranslate (whole, whole);
  }
}


void 
ExportHtml::MakeImageDir ()
{
  QDir imgdir (imagePath);
  imgdir.mkpath (imagePath);
}

QString
ExportHtml::NewImagename (QString oldname)
{
  imageCounter++;
  QFileInfo info(oldname);
  
  return imageDir + QDir::separator() +
           imageStem + QString::number(imageCounter) + 
            "." + info.suffix();
}

void 
ExportHtml::CopyImages ()
{
  ImgTranslateMap::iterator nit;
  for (nit = imgNameTranslate.begin(); nit != imgNameTranslate.end(); nit++) {
    QFile imgFile (nit->second.origFile);
    QString newname = indexPath + QDir::separator() + nit->second.newFile;
    QFile newFile (newname);
    if (newFile.exists()) {
      newFile.remove();
    }
    imgFile.copy (newname);
  }
}

void
ExportHtml::MakePaths (QString wholeIndexName)
{
  imageStem = pageTitle + "_image_";
  QFileInfo info (wholeIndexName);
  QString path = info.path();
  imagePath = path + QDir::separator() + imageDir;
  indexPath = path;
}

#endif


} // namespace

