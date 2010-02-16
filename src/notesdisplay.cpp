#include "notesdisplay.h"
#include "delib-debug.h"
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QListWidgetItem>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>

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
  SetupMenu ();
  connect (notesIndex, SIGNAL (itemActivated (QListWidgetItem*)),
           this, SLOT (UserPicked (QListWidgetItem*)));
}

void
NotesDisplay::SetupMenu ()
{
  exitAction = new QAction (tr("Exit") , this);
  menubar->addAction (exitAction);
  saveAction = new QAction (tr("Save Note"), this);
  menubar->addAction (saveAction);
  newAction = new QAction (tr("New Note"), this);
  menubar->addAction (newAction);
  
  connect (exitAction, SIGNAL (triggered()), this, SLOT (quit()));
  connect (saveAction, SIGNAL (triggered()), this, SLOT (SaveCurrent()));
  connect (newAction, SIGNAL (triggered()), this, SLOT (NewNote()));
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
NotesDisplay::Start ()
{
  FillNotesList (notesIndex);
}

void
NotesDisplay::quit ()
{
  if (pApp) {
    pApp->quit();
  }
  //ReportText ();
  //FakeSaveText ();
  StdOut() << " bye for now" << endl;
}

void
NotesDisplay::dropEvent (QDropEvent *event)
{
  qDebug () << "drop event " << event;
  
}

void
NotesDisplay::UserPicked (QListWidgetItem *item)
{
  if (item) {
    qint64 id = item->data(Qt::UserRole).toLongLong();
    QString name = item->text();
    ShowNote (id,name);
  }
}

void
NotesDisplay::ShowNote (const qint64 id, const QString & name)
{
  QString textbody;
  QString qstr ("select usergivenid, notetext from 'notes' where noteid ='");
  qstr.append (QString::number(id));
  qstr.append ("'");
  OpenDB ();
  QSqlQuery query (db);
  bool ok = query.exec (qstr);
  if (ok) {
    if (query.next()) {
      int textindex = query.record().indexOf("notetext");
      textbody = query.value(textindex).toString ();
      editBox->setHtml (textbody);
      currentId = id;
      currentName = name;
      isNew = false;
    }
  }
  
}


void
NotesDisplay::NewNote ()
{
  QDateTime now = QDateTime::currentDateTime ();
  qint64 time_id = now.toTime_t();
  QString time_str = now.toString ("yyyy-MM-dd-hh:mm:ss-Note");
  currentId = time_id;
  currentName = time_str;
  isNew = true;
  editBox->setHtml (QString("New Note"));
}

void
NotesDisplay::SaveCurrent ()
{
  if (!DBExists()) {
    MakeTables ();
  }
  OpenDB ();
  WriteNote (currentId, currentName, editBox->toHtml());
  if (isNew) {
    ListThisNote (notesIndex, currentId, currentName);
  }
}

void
NotesDisplay::ReportText ()
{
  StdOut() << editBox->toHtml ();
}

void
NotesDisplay::OpenDB ()
{
  if (db.isOpen()) {
    return;
  }
  db = QSqlDatabase::addDatabase ("QSQLITE",mConName);
  db.setDatabaseName (pConf->CompleteDBName());
  db.open ();
}

void
NotesDisplay::CloseDB ()
{
  db.close();
}

bool
NotesDisplay::DBExists ()
{
  QString name = pConf->CompleteDBName();
  QFileInfo inf (name);
  if (!inf.exists()) {
    return false;
  }
  if (!inf.isWritable()) {
    return true;
  }
  return false;
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
NotesDisplay::FillNotesList (  QListWidget * notesIndex)
{
  OpenDB ();
  QString  notesQuery ("select noteid, usergivenid from 'notes' where 1");
  QSqlQuery ListQuery (db);
  ListQuery.prepare (notesQuery);
  ListQuery.exec ();
  int idField = ListQuery.record().indexOf ("noteid");
  int nameField = ListQuery.record().indexOf ("usergivenid");
  QString name;
  qint64  id;
  while (ListQuery.next()) {
    id = ListQuery.value(idField).toLongLong();
    name = ListQuery.value(nameField).toString();
    ListThisNote (notesIndex, id, name);
  }
}

void
NotesDisplay::ListThisNote (      QListWidget *notesIndex,
                            const qint64  id,
                            const QString  &name)
{
  QListWidgetItem *item(0);
  item = new QListWidgetItem (notesIndex);
  item->setText (name);
  item->setData (Qt::UserRole, QVariant(id));
  notesIndex->addItem (item);
}

void
NotesDisplay::WriteNote (const qint64 id,
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
