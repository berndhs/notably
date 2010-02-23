#include "notaconf.h"
#include <QDesktopServices>
#include <QCoreApplication>
#include <QDir>
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

void
NotaConf::Sync ()
{
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
  return mPath + QDir::separator() + mDatafile;
}

QString
NotaConf::CompleteDBName ()
{
  return mPath + QDir::separator() + mDatafile;
}

QString
NotaConf::FileName ()
{
  return mDatafile;
}

void
NotaConf::SetFileName (QString fn)
{
  mDatafile = fn;
  Settings().setValue ("datafile", mPath);
}

void
NotaConf::SetDirectory (QString dir)
{
  mPath = dir;
  Settings().setValue ("datapath",mPath);
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
