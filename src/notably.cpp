//
/** Notably
 * @brief Note Taker program
 *
 **/
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

#include "deliberate.h"
#include "delib-debug.h"
#include "notesdisplay.h"
#include "notaconf.h"
#include "version.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDir>


int 
main (int argc, char* argv[])
{

  QStringList::iterator sit;
  
  QCoreApplication::setApplicationName ("notably");
  QCoreApplication::setOrganizationName ("BerndStramm");
  QCoreApplication::setOrganizationDomain ("bernd-stramm.com");
  deliberate::ProgramVersion pv ("Notably");
  QCoreApplication::setApplicationVersion (pv.Version());
  QSettings  settings;
  deliberate::SetSettings (settings);
  deliberate::UseMyOwnMessageHandler ();
  
  
  
  QApplication::addLibraryPath (QApplication::applicationDirPath());
  QString extrapath = QApplication::applicationDirPath() + QDir::separator() +
                     QString("sqldrivers");
  QApplication::addLibraryPath (extrapath);
  QStringList pathlist = QApplication::libraryPaths ();
  QString paths;
  for (sit= pathlist.begin(); sit!= pathlist.end(); sit++) {
    paths.append (*sit);
    paths.append (" :\n ");
  }

  paths.prepend ("Before App: Library paths: ");
  paths.prepend (QString::number(pathlist.size()) + " ");
  
  //QApplication::setLibraryPaths (QStringList());
  QStringList  drivers = QSqlDatabase::drivers();
  QString txt ("Before App: drivers:\n");
  for (sit = drivers.begin(); sit != drivers.end(); sit++) {
    txt.append (*sit);
    txt.append (" - ");
  }

  QApplication App (argc, argv);
  QMessageBox box;
  box.setText (paths);
  box.exec ();
  
  box.setText (txt);
  box.exec ();
  
  
  pv.CLIVersion ();

  paths = App.applicationDirPath();
  paths.prepend ("Application Dir: ");
  box.setText (paths);
  box.exec ();


  nota::NotesDisplay notes (App);
  nota::NotaConf     conf;
  notes.SetConf (conf);
  notes.Start ();
  notes.show ();
  
  
  App.exec ();
  return 0;
}
