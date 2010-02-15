#include "notesdisplay.h"
#include "delib-debug.h"
#include <QFile>
#include <QByteArray>

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

#include <QFile>

using namespace deliberate;

namespace nota {

NotesDisplay::NotesDisplay ()
:pApp(0),
 pConf(0),
 mConName ("nota_dbcon")
{
  setupUi (this);
  exitAction = new QAction (tr("Exit") , this);
  menubar->addAction (exitAction);
  
  connect (exitAction, SIGNAL (triggered()), this, SLOT (quit()));
}

void
NotesDisplay::SetApplication (QApplication * pA)
{
  pApp = pA;
}

void
NotesDisplay::SetConf (NotaConf & conf)
{
  pConf = &conf;
}

void
NotesDisplay::quit ()
{
  if (pApp) {
    pApp->quit();
  }
  ReportText ();
  FakeSaveText ();
  StdOut() << " bye for now" << endl;
}

void
NotesDisplay::dropEvent (QDropEvent *event)
{
  qDebug () << "drop event " << event;
  
}

void
NotesDisplay::FakeSaveText ()
{
  //MakeTables ();
  OpenDB ();
  WriteDB (2, "note title", editBox->toHtml());
  CloseDB ();
}

void
NotesDisplay::ReportText ()
{
  StdOut() << editBox->toHtml ();
}

void
NotesDisplay::OpenDB ()
{
  db = QSqlDatabase::addDatabase ("QSQLITE",mConName);
  db.setDatabaseName (pConf->CompleteDBName());
  db.open ();
}

void
NotesDisplay::CloseDB ()
{
  db.close();
}

void
NotesDisplay::MakeTable (QString table)
{
  OpenDB ();
  QString filename = QString (":qrcfiles/schema-") + table 
                      + QString (".sql");
  QFile schemafile (filename);
  schemafile.open (QFile::ReadOnly);
  QByteArray createcommands = schemafile.readAll ();
  schemafile.close ();
  QString querytext (createcommands);
  QSqlQuery qry (db);
  qry.prepare (querytext);
  qry.exec ();
  db.close();
}

void 
NotesDisplay::MakeTables ()
{
  MakeTable ("notes");
  MakeTable ("tags");
  MakeTable ("tagrefs");
  MakeTable ("identndx");
}


void
NotesDisplay::WriteDB (const qint64 id,
                       const QString & name,
                       const QString & text)
{

  QSqlQuery InsertQuery (db);
  QString queryPattern ("insert or replace into 'notes' (noteid, usergivenid, notetext)");
  queryPattern.append (" VALUES (?,?,?)");
  InsertQuery.prepare (queryPattern);
  QVariant v[3];
  v[0].setValue (id);
  v[1].setValue (name);
  v[2].setValue (text);
  for (int i=0; i<3; i++) {
    InsertQuery.bindValue (i,v[i]);
  }
  InsertQuery.exec ();
}
}
