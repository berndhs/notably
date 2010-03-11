#ifndef EXPORT_HTML_H
#define EXPORT_HTML_H

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

#if DELIBERATE_HAVE_WEBELT
#include <QWebElementCollection>
#include <QWebElement>
#include <QWebFrame>
#include <QTextEdit>
#include <QDir>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QVariant>
#include <QSet>
#include <map>
#include <QFileDialog>
#include <QDesktopServices>
#include <QWebView>
#include <map>
#endif

#include <QByteArray>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QSqlDatabase>
#include <QWidget>

namespace nota {

#if DELIBERATE_HAVE_WEBELT

class ImageNameTranslate {

public:

   ImageNameTranslate (QString old) :copied(false),origFile(old){}
   ImageNameTranslate (QString o, QString n):copied(false),
                                             origFile(o),
                                             newFile(n){}
   ImageNameTranslate ():copied(false){}
   ImageNameTranslate (const ImageNameTranslate & orig)
                       :copied (orig.copied),
                        origFile(orig.origFile),
                        newFile(orig.newFile){}

   bool     copied;
   QString  origFile;
   QString  newFile;

};

typedef std::map <QString,ImageNameTranslate> ImgTranslateMap;

#endif

class ExportHtml : public QWidget {
Q_OBJECT

public:

  ExportHtml (QWidget *parent, QSqlDatabase &db);
  
  bool ExportBook (QString bookname);
  bool ExportNote (qint64 id, QString copyToHere, QString oldPath);
  void ExportAllImages (const QString srcpath, const QString destpath);
  
  static bool IsImplemented ();


private:

  void ReadNote (qint64 id, QByteArray & noteView);
  void CopyPageImages (const qint64 notesid, 
                   const QString srcdir,
                   const QString destdir);

  #if DELIBERATE_HAVE_WEBELT
  bool PrivateExportBook (QString bookname);
  
  void GetBookNotes (QString bookname, QSet<qint64> & idset);
  void FillNoteIdSet (qint64 startId);
  QString GetNoteTitle (qint64 noteId);
  void    ConstructPages (QString indexname, 
                          QSet<qint64> & idSet,
                          std::map<qint64,QString> & nameTable);
  void  ConstructIndexpage (QString pagename,
                           QSet<qint64> &idSet,
                          std::map<qint64,QString> & nameTable,
                          QString completeName);
  void ConstructWebpage (qint64, QString path);
  QString LinkFileName (qint64 id);
  
  void TranslateImageLoc (QUrl iamgeUrl);
  void MakeImageDir ();
  void MakePaths (QString wholeIndexName);
  void CopyImages ();
  
  QString NewImagename (QString oldname);
  #endif
  
  QSqlDatabase  *pDB;

  #if DELIBERATE_HAVE_WEBELT
  QAction                     *testingAction;
  QSet <qint64>                exportSet;
  std::map <qint64, QString>   exportNames;
  QString                      imagePath;
  QString                      imageStem;
  QString                      imageDir;
  QString                      indexPath;
  QString                      pageTitle;
  int                          imageCounter;
  ImgTranslateMap              imgNameTranslate;
  
  #endif

};

} // namespace



#endif
