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
#include <QWebView>
#include <QTextEdit>
#include <QFile>
#include <QDir>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QVariant>
#include <QSet>
#include <map>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#endif

#include <QString>
#include <QSqlDatabase>
#include <QWidget>

namespace nota {

class ExportHtml : public QWidget {
Q_OBJECT

public:

  ExportHtml (QWidget *parent, QSqlDatabase &db);
  
  bool ExportBook (QString bookname);
  
  static bool IsImplemented ();


private:

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
  #endif
  
  QSqlDatabase  *pDB;

  #if DELIBERATE_HAVE_WEBELT
  QAction    *testingAction;
  QSet <qint64> exportSet;
  std::map <qint64, QString> exportNames;
  
  #endif

};

} // namespace



#endif
