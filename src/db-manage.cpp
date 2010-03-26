#include "db-manage.h"


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

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QVariant>
#include "delib-debug.h"

namespace nota {

DBManage::DBManage (QSqlDatabase & db)
{
  pDB = &db;
  tagList.clear();
  tagList << "important"
          << "dead"
          << "done";
   dbElementList << "notes"      
                 << "tags"
                 << "tagrefs"
                 << "imagerefs"
                 << "books"
                 << "bookrefs"
                 << "identndx"
                 << "lastupdate"
                 << "uniquepairs"
                 << "uniquebooktitle"
                 << "uniqueupdate"
                 << "uniquetagref"
                 << "uniquetags";
}


void 
DBManage::MakeTables ()
{
  OpenDB (conName);
  QFileInfo info (dbCompleteName);
  
  QString dirname = info.path();
  QString filename = info.fileName();
  QDir dir (dirname);
  if (!dir.exists()) {
    dir.mkpath (dirname);
  }
  QFile file (filename);
  file.open (QFile::WriteOnly);
  file.write (QString("").toLatin1(), 0);
  file.close ();
  QStringList::iterator dbit;
  for (dbit = dbElementList.begin(); dbit != dbElementList.end(); dbit++ ) {
    MakeTable (*dbit);
  }
  InitTags  ();
}

bool
DBManage::CheckTables (bool create)
{
  int goodElements (0);
  if (pDB) {
    if (!pDB->open()) {
      OpenDB (conName, create);
    }
    QStringList::iterator elit;
    QString name;
    QString kind;
    for (elit = dbElementList.begin(); elit != dbElementList.end(); elit++) {
      name = *elit;
      kind = ElementType (name).toUpper();
      if (kind != "TABLE" && kind != "INDEX") {
        if (create) {
          MakeTable (name);
        }
      }
    }
  } else {
    return false;
  }
  return goodElements == dbElementList.size();
}

void
DBManage::OpenDB (QString conName, bool createFile)
{
  if (pDB->isOpen()) {
    return;
  }
  // first make sure files is there
  QFileInfo dbinfo (dbCompleteName);
  if (!dbinfo.exists()) {
    QFile dbfile (dbCompleteName);
    bool opened = dbfile.open(QFile::WriteOnly);
    if (opened) {
      dbfile.write (QString("").toUtf8());
      dbfile.close();
    }
  }
  *pDB = QSqlDatabase::addDatabase ("QSQLITE",conName);
  pDB->setDatabaseName (dbCompleteName);
  pDB->open ();
}

QString
DBManage::ElementType (QString name)
{
  QSqlQuery query (*pDB);
  QString   cmdPat ("select * from main.sqlite_master where name=\"%1\"");
  QString   cmd = cmdPat.arg (name);
  bool ok = query.exec (cmd);
  if (ok && query.next()) {
    QString tp = query.value(0).toString();
    return tp;
  }
  return QString ("none");
}


void
DBManage::MakeTable (QString table)
{
  QString filename = QString (":qrcfiles/schema-") + table 
                      + QString (".sql");
  QFile schemafile (filename);
  schemafile.open (QFile::ReadOnly);
  QByteArray createcommands = schemafile.readAll ();
  schemafile.close ();
  QString querytext (createcommands);
  QSqlQuery qry (*pDB);
  qry.prepare (querytext);
  qry.exec ();
}

void
DBManage::InitTags ()
{
  QString filenamePattern (":img/%1.png");
  QString qryStr ("insert or replace into tags (tagname, icon) values (?, ?)");
  QStringList::iterator tagit;
  for (tagit = tagList.begin(); tagit != tagList.end(); tagit++) {
    QString name = *tagit;
    QString filename = filenamePattern.arg(name);
    QFile file(filename);
    file.open (QFile::ReadOnly);
    QByteArray imgbytes = file.readAll();
    file.close ();
    QSqlQuery query (*pDB);
    QVariant tagname;
    QVariant iconbytes;
    tagname.setValue (name);
    iconbytes.setValue (imgbytes);
    query.prepare (qryStr);
    query.bindValue (0,tagname);
    query.bindValue (1,iconbytes);
    bool ok = query.exec ();
  }
}
} // namespace

