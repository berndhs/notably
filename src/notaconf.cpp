#include "notaconf.h"
#include <QDesktopServices>
#include <QCoreApplication>
#include "delib-debug.h"

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

void
NotaConf::CheckInit ()
{
  qDebug () << "conf app name " << QCoreApplication::applicationName ();
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

bool
NotaConf::SupportPartScreenShot ()
{
  #ifdef _WIN32
  return false;
  #endif
  #ifdef Q_WS_MAEMO_5
  return false;
  #endif
  return true;
}

}
