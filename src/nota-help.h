#ifndef NOTA_HELP_H
#define NOTA_HELP_H

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
#include <QWidget>
#include "ui_helpwin.h"

namespace nota {

class NotaHelp : public QWidget, public Ui_HelpView {

Q_OBJECT

public:

  NotaHelp (QWidget *parent=0);
  ~NotaHelp ();
  
  void ShowPage (QString urlString);
  
public slots:

  void DoClose ();
  void DoBack ();
  void DoForward ();
  
  void update ();
  
private:

  void ConnectButtons ();

};

}

#endif
