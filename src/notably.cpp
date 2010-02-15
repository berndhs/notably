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
#include <QApplication>


int 
main (int argc, char* argv[])
{

  deliberate::UseMyOwnMessageHandler ();
  
  QApplication App (argc, argv);
  
  nota::NotesDisplay notes;
  nota::NotaConf     conf;
  notes.SetApplication (&App);
  notes.SetConf (conf);
  notes.show ();
  
  deliberate::StdOut () << " more later" << endl;
  
  App.exec ();
  return 0;
}
