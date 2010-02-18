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

#include "delib-debug.h"
#include "notesdisplay.h"
#include "notaconf.h"
#include "version.h"
#include <QApplication>


int 
main (int argc, char* argv[])
{

  deliberate::ProgramVersion pv ("Notably");
  
  deliberate::UseMyOwnMessageHandler ();
  
  QApplication App (argc, argv);
  
  pv.CLIVersion ();
  
  nota::NotesDisplay notes;
  nota::NotaConf     conf;
  notes.SetApplication (&App);
  notes.SetConf (conf);
  notes.Start ();
  notes.show ();
  
  
  App.exec ();
  return 0;
}
