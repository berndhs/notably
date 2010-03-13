
#include "import.h"

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
#include <QMessageBox>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include "delib-debug.h"

namespace nota {

Importer::Importer (QWidget *parent, QSqlDatabase &mine)
:QWidget (parent),
 importDialog (this),
 importLog (this),
 queryBusy (false)
{
  myDB = &mine;
  otherCon = "NotaOtherDB";
  importUI.setupUi (&importDialog);
  importLogUI.setupUi (&importLog);
  Connect ();
  InitButtons ();
}

void
Importer::Update ()
{
  importLog.update();
  update ();
}


void
Importer::Connect ()
{
  connect (importUI.startButton, SIGNAL (clicked()),
           &importDialog, SLOT (accept()));
  connect (importUI.cancelButton, SIGNAL (clicked()),
           &importDialog, SLOT (reject()));
  connect (importLogUI.doneButton, SIGNAL (clicked()),
           this, SLOT (CloseLog()));
  connect (this, SIGNAL (LogLine(QString)), this, SLOT (LineLogger(QString)));
  connect (this, SIGNAL (SigResult()),
           this, SLOT (CatchResult()));
}

void
Importer::InitButtons ()
{
  importUI.addNew->setChecked(true);
  importUI.overwriteOld->setChecked (false);
  importUI.importMergeBooks->setChecked (false);
  importUI.oneNewBook->setChecked (true);
  importUI.showLog->setChecked (false);
}

void
Importer::MergeFrom (QString otherPath)
{
  source = otherPath;
  otherDB = QSqlDatabase::addDatabase ("QSQLITE",otherCon);
  otherDB.setDatabaseName (otherPath);
  bool ok = otherDB.open ();
  if (!ok) {
    ok = QSqlDatabase::isDriverAvailable ("QSQLITE");
    QString msg = (ok ? "Sqlite driver ok" : "No QSQLITE Driver");
    QString pattern (tr("Cannot open database %1 "));
    msg.prepend (pattern.arg(otherPath));
    QMessageBox box(this);
    box.setText (msg);
    box.exec ();
    return;
  }
  importUI.importPath->setText (otherPath);
  int doimport = importDialog.exec ();
  if (doimport) {
    QFileInfo  info (otherPath);
    otherDir = info.path();
    QTimer::singleShot (50,this,SLOT(DoMerge ()));
  } else {
    otherDB.close();
  }
}

void
Importer::DoMerge ()
{
  queryBusy = true;
  emit Busy (true);
  QString  allnotes ("select noteid, usergivenid, notetext from notes where 1");
  otherQuery = QSqlQuery (otherDB);
  otherQuery.setForwardOnly (true);
  bool ok = otherQuery.exec (allnotes);
  if (!ok) {
    QMessageBox box (this);
    box.setText (tr("Query failed, bad database?"));
    box.exec ();
    return;
  }
  idNdx = otherQuery.record().indexOf ("noteid");
  titleNdx = otherQuery.record().indexOf ("usergivenid");
  bodyNdx = otherQuery.record().indexOf ("notetext");
  importLog.setUpdatesEnabled (true);
  importLogUI.messageBox->clear();
  importLogUI.sourceLine->setText (source);    
  
  //importLog.show (); 
  if (otherQuery.next()) {
    emit SigResult ();
  } else {
    otherDB.close();
  }
}

void
Importer::CatchResult ()
{
  QString logline;
  QString mergedPat (tr("merged %2 \"%1\""));
  QString skippedPat (tr("skipped \"%1\""));
  qint64 id;
  QString title;
  QString body;
  bool copyIt (false);
  uint myTime, otherTime;
  id = otherQuery.value(idNdx).toLongLong();
  title = otherQuery.value(titleNdx).toString();
  body = otherQuery.value(bodyNdx).toString();
  otherTime = TimeStamp (otherDB, id);
  if (NoteExists (*myDB, id)) {
    myTime = TimeStamp (*myDB, id);
    copyIt = (myTime < otherTime);
  } else {
    copyIt = true;
  }
  if (copyIt) {
    QSqlQuery transact (*myDB);
    transact.exec ("BEGIN TRANSACTION");
    bool ok = WriteNote (*myDB, id, title, body, otherTime);
    if (ok) {
      ok &= MergeAllRefs (*myDB, otherDB, id);
    }
    transact.exec ("COMMIT TRANSACTION");
    logline = mergedPat.arg(title).arg((ok ? "good " : "bad? "));
  } else {
    logline = skippedPat.arg(title);  
  }
  emit LogLine (logline);
  if (otherQuery.next()) {
    emit SigResult();
  } else {
    otherQuery.finish ();
    otherDB.close();
    queryBusy = false;
    emit Busy (false);
    importLog.show();
  }
}

void
Importer::LineLogger (QString line)
{
  importLogUI.messageBox->append (line);
  Update ();
}

void
Importer::update ()
{
  importLog.update();
  QWidget::update();
}

bool
Importer::WriteNote (QSqlDatabase & db, 
                     const qint64    noteid,
                     const QString  &usergivenid,
                     const QString  &notetext,
                     const uint      timestamp)
{
  QSqlQuery myQuery (db);
  QString  myCmd ("insert or replace into notes "          
                    "( noteid, usergivenid, notetext )"
                    " values (?,?,?)");    
  myQuery.prepare (myCmd);
  myQuery.bindValue (0, noteid);
  myQuery.bindValue (1, usergivenid);
  myQuery.bindValue (2, notetext);
  bool ok = myQuery.exec ();
  myCmd = QString ("insert or replace into lastupdate (noteid, updatetime ) "
                   "values (?,?)");
  myQuery.prepare (myCmd);
  myQuery.bindValue (0,noteid);
  myQuery.bindValue (1,timestamp);
  ok &= myQuery.exec ();
  return ok;
}

uint
Importer::TimeStamp (QSqlDatabase &db, qint64 noteid)
{
  QString cmdPat ("select updatetime from lastupdate where noteid =%1 ");
  QString cmdQuery = cmdPat.arg(noteid);
  QSqlQuery timeQuery (db);
  bool ok = timeQuery.exec (cmdQuery);
  if (ok && timeQuery.next()) {
    return timeQuery.value(0).toUInt();
  }
  return 0;
}

bool
Importer::NoteExists (QSqlDatabase &db, qint64 noteid)
{
  QString cmdPat ("select count(noteid) from notes where noteid=%1");
  QString cmdStr = cmdPat.arg(QString::number(noteid));
  QSqlQuery countQuery (db);
  bool ok = countQuery.exec (cmdStr);
  if (ok && countQuery.next()) {
    int count = countQuery.value(0).toInt();
    return count;
  } else {
    return 0;
  }
  
}

bool
Importer::MergeAllRefs (QSqlDatabase & toDB,
                     QSqlDatabase & fromDB,
                     qint64         noteid)
{
  bool ok;
  ok = MergeRefs (toDB, fromDB, "tagrefs", "tagname", noteid);
  ok &= MergeRefs (toDB, fromDB, "imagerefs", "imageref", noteid);
  ok &= MergeRefs (toDB, fromDB, "bookrefs", "bookname", noteid);
  return ok;
}

bool
Importer::MergeRefs (QSqlDatabase  & toDB,
                  QSqlDatabase  & fromDB,
                  QString         tablename,
                  QString         fieldname,
                  qint64          noteid)
{
  QString readPattern ("select distinct %1 from %2 where noteid = %3");
  QString writePattern ("insert or replace into %1 (noteid, %2)"
                        "values (?,?)");
  QString readCmd = readPattern.arg(fieldname).arg(tablename).arg(noteid);
  QString writeCmd = writePattern.arg(tablename).arg(fieldname);
  QSqlQuery readQuery (fromDB);
  QSqlQuery writeQuery (toDB);
  bool ok = readQuery.exec (readCmd);
  QString ref;
  while (ok && readQuery.next()) {
    ref = readQuery.value (0).toString();
    writeQuery.prepare (writeCmd);
    writeQuery.bindValue (0,noteid);
    writeQuery.bindValue (1,ref);
    writeQuery.exec ();
  }
  return ok;
}


void
Importer::CloseLog ()
{
  importLog.reject ();
}

} // namespace


