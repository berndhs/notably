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
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>


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


  QApplication App (argc, argv);
#if 0
  QMessageBox box;

  QString appmsg ("This is ");
  appmsg.append (QCoreApplication::applicationName());
  appmsg.append (" by ");
  appmsg.append (QCoreApplication::organizationName());
  box.setText (appmsg);
  box.exec ();
#endif

  QApplication::addLibraryPath (QApplication::applicationDirPath());
  QString extrapath = QApplication::applicationDirPath() + QDir::separator() +
                     QString("sqldrivers");
  QApplication::addLibraryPath (extrapath);
#if 0
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

  box.setText (paths);
  box.exec ();
  
  box.setText (txt);
  box.exec ();
#endif
  QTranslator  trans;
  QString tname = "qt_" + QLocale::system().name();
  QString loc = QLibraryInfo::location (QLibraryInfo::TranslationsPath);
  bool langok = trans.load (tname, loc);
  App.installTranslator (&trans);
  
  QTranslator  notaTrans;
  tname = "notably_" +QLocale::system().name();
  langok = notaTrans.load (tname,":/viewer");
  App.installTranslator (&notaTrans);
  pv.CLIVersion ();

#if 0
  paths = App.applicationDirPath();
  paths.prepend ("Application Dir: ");
  box.setText (paths);
  box.exec ();
#endif

  nota::NotesDisplay notes (App);
  nota::NotaConf     conf;
  notes.SetConf (conf);
  notes.Start ();
  notes.show ();
  
  
  App.exec ();
  return 0;
}
