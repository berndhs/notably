
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
 importLog (this)
{
  myDB = &mine;
  otherCon = "NotaOtherDB";
  importUI.setupUi (&importDialog);
  importLogUI.setupUi (&importLog);
  Connect ();
  InitButtons ();
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
qDebug () << " Importer::MergeFrom " << otherPath;
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
  qDebug () << " db open ok " << ok;
  importUI.importPath->setText (otherPath);
  int doimport = importDialog.exec ();
  if (doimport) {
    QFileInfo  info (otherPath);
    otherDir = info.path();
    DoMerge ();
  } else {
    qDebug () << " they quit ";
  }
  otherDB.close();
  qDebug () << " closed other db";
}

void
Importer::DoMerge ()
{
  qDebug () << " addNew " << importUI.addNew->isChecked();
  qDebug () << " overwriteOld " << importUI.overwriteOld->isChecked();
  qDebug () << " importMergeBooks " << importUI.importMergeBooks->isChecked();
  qDebug () << " oneNewBook " << importUI.oneNewBook->isChecked ();
  qDebug () << " show log " << importUI.showLog->isChecked();
  QString  allnotes ("select noteid, usergivenid, notetext from notes where 1");
  QSqlQuery otherQuery (otherDB)  ;
  bool ok = otherQuery.exec (allnotes);
  if (!ok) {
    QMessageBox box (this);
    box.setText (tr("Query failed, bad database?"));
    box.exec ();
    return;
  }
  int idNdx = otherQuery.record().indexOf ("noteid");
  int titleNdx = otherQuery.record().indexOf ("usergivenid");
  int bodyNdx = otherQuery.record().indexOf ("notetext");
  QString logline;
  QString pattern ("merged \"%1\"");
  qint64 id;
  QString title;
  QString body;
  importLog.show ();
  importLogUI.sourceLine->setText (source);
  while (otherQuery.next()) {
    id = otherQuery.value(idNdx).toLongLong();
    title = otherQuery.value(titleNdx).toString();
    body = otherQuery.value(bodyNdx).toString();
    logline = pattern.arg(title);
    importLogUI.messageBox->append (logline);
  }
}

void
Importer::CloseLog ()
{
  importLog.reject ();
}

} // namespace


