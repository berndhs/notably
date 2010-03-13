#ifndef IMPORT_H
#define IMPORT_H

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

#include <QWidget>
#include <QString>
#include <QSqlDatabase>
#include <QTimer>
#include <QSqlQuery>
#include "ui_import.h"
#include "ui_importlog.h"


namespace nota {

class Importer : public QWidget {
Q_OBJECT

public:

Importer (QWidget *parent, QSqlDatabase &mine);

bool IsBusy () { return queryBusy; }

public slots:

void MergeFrom (QString otherPath);
void update ();

private slots:

  void CloseLog ();
  void Update ();
  void LineLogger (QString line);
  void CatchResult ();
  void DoMerge ();

signals:
 
  void LogLine (QString line);
  void SigResult ();
  void Busy (bool busy);

private:

  void Connect ();
  void InitButtons ();
 
  uint TimeStamp (QSqlDatabase & db, qint64 noteid);
  bool NoteExists (QSqlDatabase &db, qint64 noteid);
 
  bool WriteNote (QSqlDatabase & db, 
                 const qint64    noteid,
                 const QString  &usergivenid,
                 const QString  &notetext,
                 const uint      timestamp);
  bool MergeAllRefs (QSqlDatabase & toDB,
                  QSqlDatabase & fromDB,
                  qint64        noteid);
  bool MergeRefs (QSqlDatabase  & toDB,
                  QSqlDatabase  & fromDB,
                  QString         tablename,
                  QString         fieldname,
                  qint64          noteid);

  QSqlDatabase    * myDB;
  QSqlDatabase    otherDB;
 
  QString        otherCon;
 
  QDialog           importDialog;
  Ui_ImportDialog   importUI;
  QDialog           importLog;
  Ui_ImportLog      importLogUI;
 
  QString    otherDir;
  QString    source;
  
  QSqlQuery  otherQuery;
  int        idNdx;
  int        titleNdx;
  int        bodyNdx;
  bool       queryBusy;
  
};


} // namespace

#endif
