#ifndef DB_MANAGE_H
#define DB_MANAGE_H

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

#include "deliberate.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include "notaconf.h"

namespace nota {

class DBManage {

public:

  DBManage (QSqlDatabase & db);
  
  void SetConf (NotaConf * pC) { pConf = pC; }
    
  void MakeTables (QString connectionName);
  void InitTags ();
  
private:

  void MakeTable (QString table);
  void OpenDB (QString conName);

  QSqlDatabase   *pDB;
  NotaConf       *pConf;
  QStringList    tagList;
  QStringList    dbElementList;

};

}

#endif
