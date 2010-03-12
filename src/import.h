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
#include "ui_import.h"
#include "ui_importlog.h"


namespace nota {

class Importer : public QWidget {
Q_OBJECT

public:

Importer (QWidget *parent, QSqlDatabase &mine);

public slots:

void MergeFrom (QString otherPath);

private slots:

void CloseLog ();

private:

 void Connect ();
 void InitButtons ();
 void DoMerge ();

 QSqlDatabase    * myDB;
 QSqlDatabase    otherDB;
 
 QString        otherCon;
 
 QDialog           importDialog;
 Ui_ImportDialog   importUI;
 QDialog           importLog;
 Ui_ImportLog      importLogUI;
 
 QString    otherDir;
 QString    source;


};


} // namespace

#endif
