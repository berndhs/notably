#include "notaconf.h"
#include <QDesktopServices>

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


namespace nota {

NotaConf::NotaConf ()
{
  QString homedir = QDesktopServices::storageLocation
                       (QDesktopServices::HomeLocation);
  mPath = homedir + QString ("/data/nota");
 
  mDatafile = QString ("nota.sql");
}

QString 
NotaConf::Directory ()
{
  return mPath;
}


QString
NotaConf::DataFile ()
{
  return mPath + "/" + mDatafile;
}

QString
NotaConf::CompleteDBName ()
{
  return mPath + "/" + mDatafile;
}

}
