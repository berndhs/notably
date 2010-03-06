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

  QCoreApplication::setApplicationName ("notably");
  QCoreApplication::setOrganizationName ("BerndStramm");
  QCoreApplication::setOrganizationDomain ("bernd-stramm.com");
  deliberate::ProgramVersion pv ("Notably");
  QCoreApplication::setApplicationVersion (pv.Version());
  QSettings  settings;
  deliberate::SetSettings (settings);
  deliberate::UseMyOwnMessageHandler ();
  
  
  QApplication App (argc, argv);
  
  pv.CLIVersion ();
  
  App.addLibraryPath (App.applicationDirPath());
  QString extrapath = App.applicationDirPath() + QDir::separator() +
                     QString("sqldrivers");
  App.addLibraryPath (extrapath);
  QStringList pathlist = App.libraryPaths ();
  QStringList::iterator sit;
  QString paths;
  for (sit= pathlist.begin(); sit!= pathlist.end(); sit++) {
    paths.append (*sit);
    paths.append (" :\n ");
  }
  QMessageBox box;

  paths.prepend ("Library paths: ");
  paths.prepend (QString::number(pathlist.size()) + " ");
  box.setText (paths);
  box.exec ();

  paths = App.applicationDirPath();
  paths.prepend ("Application Dir: ");
  box.setText (paths);
  box.exec ();
  QStringList  drivers = QSqlDatabase::drivers();

  QString txt ("drivers:\n");
  for (sit = drivers.begin(); sit != drivers.end(); sit++) {
    txt.append (*sit);
    txt.append (" - ");
  }
  box.setText (txt);
  box.exec ();

  nota::NotesDisplay notes (App);
  nota::NotaConf     conf;
  notes.SetConf (conf);
  notes.Start ();
  notes.show ();
  
  
  App.exec ();
  return 0;
}
