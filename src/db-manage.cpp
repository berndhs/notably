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
#include <QDir>
#include <QByteArray>
#include <QVariant>
#include "delib-debug.h"

namespace nota {

DBManage::DBManage (QSqlDatabase & db)
:pConf (0)
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
                 << "uniquetags";
}


void 
DBManage::MakeTables (QString connectionName)
{
  OpenDB (connectionName);
  QString dirname = pConf->Directory();
  QDir dir (dirname);
  if (!dir.exists()) {
    dir.mkpath (dirname);
  }
  QString filename = pConf->DataFile ();
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

void
DBManage::OpenDB (QString conName)
{
  if (pDB->isOpen()) {
    return;
  }
  *pDB = QSqlDatabase::addDatabase ("QSQLITE",conName);
  pDB->setDatabaseName (pConf->CompleteDBName());
  pDB->open ();
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

