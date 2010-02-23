#include "notaconf.h"
#include <QDesktopServices>
#include <QCoreApplication>
#include "delib-debug.h"
#include "deliberate.h"

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

using namespace deliberate;

namespace nota {

NotaConf::NotaConf ()
{
  QString homedir = QDesktopServices::storageLocation
                       (QDesktopServices::HomeLocation);
                       
  //mPath = homedir + QString ("/data/nota");
  mPath = QDesktopServices::storageLocation (QDesktopServices::DataLocation);
  if (Settings().contains("datapath")) {
    mPath = Settings().value("datapath", mPath).toString();
  }
 
  mDatafile = QString ("nota.sql");
  if (Settings().contains("datafile")) {
    mDatafile = Settings().value ("datafile", mDatafile).toString();
  }
}

NotaConf::~NotaConf ()
{
  Settings().setValue ("datapath",mPath);
  Settings().setValue ("datafile",mDatafile);
  Settings().sync();
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
