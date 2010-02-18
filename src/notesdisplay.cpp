#include "notesdisplay.h"
#include "delib-debug.h"
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QListWidgetItem>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <QMessageBox>

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
 noteMenu(this),
 editMenu(this),
 mConName ("nota_dbcon")
{
  setupUi (this);
  SetupMenu ();
  SetupEdit ();
  ShowNothing ();
  fontProperty[FP_bold] = false;
  fontProperty[FP_italic] = false;
  fontProperty[FP_underline] = false;
  connect (notesIndex, SIGNAL (itemActivated (QListWidgetItem*)),
           this, SLOT (UserPicked (QListWidgetItem*)));
  connect (noteName, SIGNAL (textEdited (const QString &)), 
           this, SLOT (NameChanged(const QString &)));
           
  connect (&debugTimer, SIGNAL (timeout()), this, SLOT (DebugCheck()));
 // debugTimer.start (1000);
}

void
NotesDisplay::SetupMenu ()
{
  exitAction = new QAction (tr("Exit") , this);
  menubar->addAction (exitAction);
  saveAction = new QAction (tr("Save Note"), this);
  menubar->addAction (saveAction);
  noteMenuAction = new QAction (tr("Note..."), this);
  menubar->addAction (noteMenuAction);
  editAction = new QAction (tr("Edit..."), this);
  menubar->addAction (editAction);
  helpAction = new QAction (tr("Help"), this);
  menubar->addAction (helpAction);
  
  connect (exitAction, SIGNAL (triggered()), this, SLOT (quit()));
  connect (saveAction, SIGNAL (triggered()), this, SLOT (SaveCurrent()));
  connect (noteMenuAction, SIGNAL (triggered()), this, SLOT (ShowNoteMenu()));
  connect (editAction, SIGNAL (triggered()), this, SLOT (ScheduleEdit()));
  connect (helpAction, SIGNAL (triggered()), this, SLOT (Help()));
  
  connect (&noteMenu, SIGNAL (SaveNote()), this, SLOT (SaveCurrent()));
  connect (&noteMenu, SIGNAL (PublishNote()), this, SLOT (PublishCurrent()));
  connect (&noteMenu, SIGNAL (DeleteNote()), this, SLOT (DeleteCurrent()));
  connect (&noteMenu, SIGNAL (NewNote()), this, SLOT (NewNote()));
  connect (&noteMenu, SIGNAL (CancelNote()), this, SLOT (ShowNothing()));
}

void
NotesDisplay::SetupEdit ()
{
  connect (&editMenu, SIGNAL (SigFontToggle (const FontProperty)),
            this, SLOT (ToggleFont (const FontProperty)));
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
}

void
NotesDisplay::dropEvent (QDropEvent *event)
{
  qDebug () << "drop event " << event;
  
}

void
NotesDisplay::ToggleFont (const FontProperty prop)
{
  if (prop <= FP_none || prop >= FP_max) {
    return;
  }
  bool newVal = !fontProperty[prop];
  fontProperty[prop] = newVal;
  switch (prop) {
  case FP_bold:
    editBox->setFontWeight (newVal ? QFont::Bold : QFont::Normal);
    break;
  case FP_italic:
    editBox->setFontItalic (newVal);
    break;
  case FP_underline:
    editBox->setFontUnderline (newVal);
    break;
  default:
    break;
  }
}

void
NotesDisplay::Help ()
{
  deliberate::ShowVersionWindow ();
}

void
NotesDisplay::ShowNoteMenu ()
{
  actionTimer.setSingleShot (true);
  connect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecNoteMenu()));
  actionTimer.start (100);
}

void
NotesDisplay::ScheduleEdit ()
{
  actionTimer.setSingleShot (true);
  connect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecEditMenu()));
  actionTimer.start (100);
}

void
NotesDisplay::ExecEditMenu ()
{
  disconnect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecEditMenu()));
  editMenu.Exec (editBox->mapToGlobal(QPoint(0,0)));
}

void
NotesDisplay::ExecNoteMenu ()
{
  disconnect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecNoteMenu()));
  noteMenu.Exec (editBox->mapToGlobal(QPoint(0,0)));
}

void
NotesDisplay::UserPicked (QListWidgetItem *item)
{
  if (item) {
    qint64 id = item->data(Qt::UserRole).toLongLong();
    QString name = item->text();
    ShowNote (item,id,name);
  }
}

void
NotesDisplay::DeleteFromDB (const qint64 id)
{
  OpenDB ();
  QString delStr ("delete from notes where noteid ='");
  delStr.append (QString::number(id));
  delStr.append ("'");
  QSqlQuery qry (db);
  qry.exec (delStr);
}

void
NotesDisplay::DeleteCurrent ()
{
  if (showingNote) {
    DeleteFromDB (currentId);
    if (curItem) {
      int row = notesIndex->row (curItem);
      QListWidgetItem * deadItem = notesIndex->takeItem (row);
      if (deadItem) {
        delete deadItem;
        curItem = 0;
      }
    }
  }
  ShowNothing();
}

void
NotesDisplay::ShowNote (QListWidgetItem *item,
                        const qint64 id, 
                        const QString & name)
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
      noteName->setText (name);
      editBox->setHtml (textbody);
      showingNote = true;
      curItem = item;
      currentId = id;
      currentName = name;
      newName = currentName;
      nameChanged = false;
      isNew = false;
    }
  }
  
}

void
NotesDisplay::ShowNothing ()
{
  isNew = false;
  currentId = 0;
  currentName = "";
  newName = "";
  nameChanged = false;
  noteName ->setText ("");
  editBox->setHtml ("");
  curItem = 0;
  showingNote = false;
}

void
NotesDisplay::MakeNew (qint64 & id, QString &name)
{
  QDateTime now = QDateTime::currentDateTime ();
  id = now.toTime_t();
  name = now.toString ("yyyy-MM-dd-hh:mm:ss-Note");
}

void
NotesDisplay::NewNote ()
{
  MakeNew (currentId, currentName);
  newName = currentName;
  nameChanged = false;
  curItem = 0;
  isNew = true;
  noteName->setText (currentName);
  editBox->setHtml (QString("New Note"));
  showingNote = true;
}


void
NotesDisplay::NameChanged (const QString & name)
{
  if (currentName != name) {
    nameChanged = true;
    newName = name;
  }
}

void
NotesDisplay::SaveCurrent ()
{
  if (!DBExists()) {
    MakeTables ();
  }
  OpenDB ();
  if (!showingNote) {
    MakeNew (currentId, currentName);
    isNew = true;
  }
  if (nameChanged) {
    currentName = newName;
  }
  WriteNote (currentId, currentName, editBox->toHtml());
  if (isNew) {
    curItem = ListThisNote (notesIndex, currentId, currentName);
    noteName->setText (currentName);
  } else if (nameChanged) {
    curItem->setText (currentName);
  }
}

void
NotesDisplay::PublishCurrent ()
{
  QString wholeName (pConf->Directory() + "/" + noteName->text() + ".html");
  QFile pageFile (wholeName);
  pageFile.open (QFile::WriteOnly);
  pageFile.write (editBox->toHtml().toLocal8Bit());
  pageFile.close();
  QMessageBox report;
  QString savedMessage = QString (tr("Note saved as %1"))
                            .arg (wholeName);
  report.setText (savedMessage);
  QTimer::singleShot (20000, &report, SLOT(accept()));
  report.exec ();
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

QListWidgetItem *
NotesDisplay::ListThisNote (      QListWidget *notesIndex,
                            const qint64  id,
                            const QString  &name)
{
  QListWidgetItem *item(0);
  item = new QListWidgetItem (notesIndex);
  item->setText (name);
  item->setData (Qt::UserRole, QVariant(id));
  notesIndex->addItem (item);
  return item;
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


void
NotesDisplay::DebugCheck ()
{
  //qDebug () << " debug timer check " << time (0);
  qDebug () << " menubar enabled " << menubar->isEnabled ();
  qDebug () << " menubar active " << menubar->activeAction ();
}

}
