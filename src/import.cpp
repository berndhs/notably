
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
#include <QDir>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QDateTime>
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
  emit Busy (queryBusy);
  bookList.clear();
  imageList.clear();
  tagList.clear();
  QString  allnotes ("select noteid from notes where 1");
  QSqlQuery otherQuery (otherDB);
  otherQuery.setForwardOnly (true);
  bool ok = otherQuery.exec (allnotes);
  if (!ok) {
    QMessageBox box (this);
    box.setText (tr("Query failed, bad database?"));
    box.exec ();
    return;
  }
  int idNdx = otherQuery.record().indexOf ("noteid");
  importLog.setUpdatesEnabled (true);
  importLogUI.messageBox->clear();
  importLogUI.sourceLine->setText (source);    
  
  importLog.show (); 
  importLog.update ();
  idSet.clear();
  while (otherQuery.next()) {
    idSet.insert (otherQuery.value(idNdx).toLongLong());
  }
  if (idSet.size() > 0) {
    emit SigResult ();
  } else {
    FinishMerge ();
  }
}

void
Importer::FinishMerge ()
{
  MergeBooks (*myDB, otherDB, bookList);
  bookList.clear ();
  MergeTags (*myDB, otherDB, tagList);
  tagList.clear ();
  MergeImages (*myDB, otherDB, imageList);
  imageList.clear ();
  otherDB.close ();
  queryBusy = false;
  emit Busy (queryBusy);
  //importLog.show();
}

void
Importer::CatchResult ()
{
  static int catchCount(0);
  if ((catchCount++) >= 10) {
    //
    // Load leveling - if we never delay, display updates will hang
    // 
    catchCount = 0;
    QTimer::singleShot (50,this, SLOT(CatchResult()));
    return;
  }
  QString logline;
  QString mergedPat (tr("merged %2 \"%1\""));
  QString skippedPat (tr("skipped \"%1\""));
  qint64 id;
  QString title;
  QString body;
  uint myTime, otherTime;
  NoteIdSetType::iterator nit = idSet.begin();
  if (nit != idSet.end()) {
    id = *nit;
    idSet.erase (nit);
  } else {
    FinishMerge ();
    return;
  }
  bool copyIt (false);
  bool readok = ReadNote (otherDB, id, title, body, otherTime);
  if (readok && NoteExists (*myDB, id)) {
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
    logline = mergedPat.arg(title).arg((ok ? tr("good ") : tr("bad? ")));
  } else {
    logline = skippedPat.arg(title);  
  }
  emit LogLine (logline);
  if (idSet.count() > 0) {
    emit SigResult ();
  } else {
    FinishMerge ();
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

bool 
Importer::ReadNote (QSqlDatabase & db, 
                 const qint64     noteid,
                       QString  & usergivenid,
                       QString  & notetext,
                       uint     & timestamp)
{
  QSqlQuery readQuery (db);
  QString cmdPat ("select usergivenid, notetext from notes where noteid =%1");
  QString cmd = cmdPat.arg (QString::number(noteid));
  usergivenid.clear();
  notetext.clear();
  readQuery.exec (cmd);
  if (readQuery.next()) {
    int titleNdx = readQuery.record().indexOf ("usergivenid");
    int bodyNdx  = readQuery.record().indexOf ("notetext");
    usergivenid = readQuery.value(titleNdx).toString();
    notetext = readQuery.value(bodyNdx).toString();
    cmdPat = QString("select updatetime from lastupdate where noteid =%1");
    cmd = cmdPat.arg(QString::number(noteid));
    readQuery.exec (cmd);
    if (readQuery.next()) {
      timestamp = readQuery.value(0).toInt();
    } else {
      timestamp = 0;
    }
    return true;
  } else {
    return false;
  }
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
    return count > 0;
  } else {
    return false;
  }
  
}

bool
Importer::KeyExists (QSqlDatabase &db, 
                     const QString table, 
                     const QString keyField,
                     const QString keyVal)
{
  QSqlQuery query (db);
  QString  pat ("select count(*) from %1 where \"%2\" = \"%3\"");
  QString  cmd = pat.arg(table).arg(keyField).arg(keyVal);
  bool ok = query.exec (cmd);
  if (ok && query.next()) {
    return (0 < query.value(0).toInt());
  } 
  return false;
}

bool
Importer::MergeAllRefs (QSqlDatabase & toDB,
                     QSqlDatabase & fromDB,
                     qint64         noteid)
{
  bool ok;
  ok = MergeRefs (toDB, fromDB, "tagrefs", "tagname", noteid, tagList);
  ok &= MergeRefs (toDB, fromDB, "imagerefs", "imageref", noteid, imageList);
  ok &= MergeRefs (toDB, fromDB, "bookrefs", "bookname", noteid, bookList);
  return ok;
}

bool
Importer::MergeRefs (QSqlDatabase  & toDB,
                  QSqlDatabase  & fromDB,
                  QString         tablename,
                  QString         fieldname,
                  qint64          noteid,
                  QSet<QString>    & refList)
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
    refList << ref;
  }
  return ok;
}

bool
Importer::MergeBooks (QSqlDatabase & toDB,
                      QSqlDatabase & fromDB,
                      QSet<QString>  & refs)
{
  if (refs.count() < 1) {
    return true; // vacuously worked
  }
  QSqlQuery  readQuery (fromDB);
  QSqlQuery  writeQuery (toDB);
  QString readPat ("select description from books where bookname =\"%1\"");
  QString readCmd;
  QString writeCmd ("insert or replace into books (bookname, description)"
                    " values (?,?)");
  bool ok (false);
  QString name;
  QString desc;
  QString logline;
  QSet<QString>::iterator it;
  for (it = refs.begin(); it != refs.end(); ++it) {
    name = *it;
    if (!KeyExists (toDB, "books", "bookname", name)) {
      readCmd = readPat.arg(name);
      ok = readQuery.exec (readCmd);
      if (ok && readQuery.next()) {
        desc = readQuery.value(0).toString();
        writeQuery.prepare (writeCmd);
        writeQuery.bindValue (0, name);
        writeQuery.bindValue (1, desc);
        ok = writeQuery.exec ();
        if (ok) {
          logline = QString("copied book %1").arg(name);
        } else {
          logline = QString ("failed to copy book %1").arg(name);
        }
        emit LogLine (logline);
      }
    }
  }
  return true;
}

bool
Importer::MergeTags (QSqlDatabase & toDB,
                      QSqlDatabase & fromDB,
                      QSet<QString>  & refs)
{
  if (refs.count() < 1) {
    return true; // vacuously worked
  }
  QSqlQuery  readQuery (fromDB);
  QSqlQuery  writeQuery (toDB);
  QString readPat ("select description, icon from tags where tagname =\"%1\"");
  QString readCmd;
  QString writeCmd ("insert or replace into tags (tagname, description, icon)"
                    " values (?,?,?)");
  bool ok (false);
  QString name;
  QString desc;
  QByteArray iconBytes;
  QString logline;
  int     descNdx = 0;
  int     iconNdx = 1;
  QSet<QString>::iterator it;
  for (it = refs.begin(); it != refs.end(); ++it) {
    name = *it;
    if (!KeyExists (toDB, "tags", "tagname", name)) {
      readCmd = readPat.arg(name);
      ok = readQuery.exec (readCmd);
      if (ok && readQuery.next()) {
        desc = readQuery.value(descNdx).toString();
        iconBytes = readQuery.value(iconNdx).toByteArray();
        writeQuery.prepare (writeCmd);
        writeQuery.bindValue (0, name);
        writeQuery.bindValue (1, desc);
        writeQuery.bindValue (2, iconBytes);
        ok = writeQuery.exec ();
        if (ok) {
          logline = QString("copied tag %1").arg(name);
        } else {
          logline = QString ("failed to copy tag %1").arg(name);
        }
        emit LogLine (logline);
      }
    }
  }
  return true;
}

bool
Importer::MergeImages (QSqlDatabase & toDB,
                      QSqlDatabase & fromDB,
                      QSet<QString>  & refs)
{
  QFileInfo destInfo (toDB.databaseName());
  QFileInfo srcInfo (fromDB.databaseName());
  QString destPath = destInfo.canonicalPath();
  QString srcPath = srcInfo.canonicalPath();
  if (destPath == srcPath) {
    return true;  // same place, nothing to copy
  }
  QString name;
  QString destFile;
  QString srcFile;
  QDateTime destTime;
  QDateTime srcTime;
  bool   copyit (false);
  bool   removeit (false);
  
  QSet<QString>::iterator it;
  for (it = refs.begin(); it != refs.end(); ++it) {
    name = *it;
    destFile = destPath + QDir::separator() + name;
    srcFile = srcPath + QDir::separator() + name;
    destInfo.setFile (destFile);
    srcInfo.setFile (srcFile);
    removeit = false;
    if (srcInfo.exists()) {
      if (destInfo.exists()) {
        destTime = destInfo.lastModified();
        srcTime = srcInfo.lastModified();
        copyit = destTime < srcTime;
        removeit = true;
      } else {
        copyit = true;
      }
    } else {
      copyit = false;
    }
    if (copyit) {
      QString wholeDir = destInfo.path();
      QDir destDir (wholeDir);
      if (!destDir.exists()) {
        bool didit = destDir.mkpath (wholeDir);
      }
      if (removeit) {
        QFile::remove (destFile);
      }
      bool ok = QFile::copy (srcFile, destFile);  
      QString basicname =  srcInfo.fileName();
      QString logline;
      if (ok) {
        logline = QString("copied image %1").arg(basicname);
      } else {
        logline = QString ("failed to copy image %1").arg(basicname);
      }
      emit LineLogger (logline);
    }
  }
  return true;
}

void
Importer::CloseLog ()
{
  importLog.reject ();
}

} // namespace


