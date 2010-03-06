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


int 
main (int argc, char* argv[])
{

  QCoreApplication::setApplicationName ("notably");
  QCoreApplication::setOrganizationName ("BerndStramm");
  QCoreApplication::setOrganizationDomain ("bernd-stramm.com");
  deliberate::ProgramVersion pv ("Notably");
  QSettings  settings;
  deliberate::SetSettings (settings);
  deliberate::UseMyOwnMessageHandler ();
  
  
  QApplication App (argc, argv);
  
  pv.CLIVersion ();
  
  App.addLibraryPath (".");
  App.addLibraryPath (QDesktopServices::storageLocation
                   (QDesktopServices::HomeLocation)
                   + "/bin");
  QStringList pathlist = App.libraryPaths ();
  QStringList::iterator sit;
  QString paths;
  for (sit= pathlist.begin(); sit!= pathlist.end(); sit++) {
   paths.append (*sit);
   paths.append (" : ");
  }
  QMessageBox box;
  paths.prepend ("Library paths:\n");
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
