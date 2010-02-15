#ifndef NOTA_EDITBOX_H
#define NOTA_EDITBOX_H


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

#include <QTextEdit>
#include <QDropEvent>

namespace nota {

class EditBox : public QTextEdit {
public:

  EditBox (QWidget *parent);

  void dropEvent (QDropEvent * event);
  void dragEnterEvent (QDragEnterEvent *event);
  
public slots:

  void paste ();
};

}



#endif
