#ifndef NOTACONF_H
#define NOTACONF_H

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


#include <QString>

namespace nota {

class NotaConf {

public:

  NotaConf ();
  ~NotaConf ();

  QString  DataFile ();
  QString  CompleteDBName ();
  QString  Directory ();
  QString  ImagePath ();
  QString  CompleteImagePath ();
  bool     SupportPartScreenShot ();
  
  QString FileName ();
  
  void    SetFileName (QString fn);
  void    SetDirectory (QString dir);
  void    SetImagePath (QString dir);
  void    Sync ();
  
private:

  QString    mPath;
  QString    mDatafile;
  QString    mImgPath;
  
};

}


#endif
