#ifndef HELPBOX_H
#define HELPBOX_H
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

#include <QDialog>
#include "ui_helpbox.h"

namespace nota {

class HelpBox : public QDialog, public Ui_HelpDialog {

Q_OBJECT

public:

  HelpBox (QWidget *parent=0);
  void setText (const QString &text);
  
  
private slots:

  void DoOk ();
  void DoHelp ();
  void DoLicense ();
 
signals:

  void WantLicense ();
  void WantHelp ();

private:

  void Connect ();

};


}


#endif
