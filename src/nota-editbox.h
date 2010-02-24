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
#include <QImage>
#include "notaconf.h"

namespace nota {

class EditBox : public QTextEdit {
Q_OBJECT
public:

  EditBox (QWidget *parent);
  
  void SetConf (NotaConf *pc) { pConf = pc; }

  void dropEvent (QDropEvent * event);
  void dragEnterEvent (QDragEnterEvent *event);
  void InsertImage (QImage & img);
  QString BoxText ();
  
  void insertFromMimeData ( const QMimeData * source );
  void mousePressEvent ( QMouseEvent * event );
  
  void GrabHtml ();
  void GrabLink ();
  
public slots:

  void paste ();

signals:

  void NewImage (QString filename);
  
private:

  void IsolateUrls (QDropEvent *event);
  QString ImgFilename (QImage & img);
  
  NotaConf *pConf;
  
};

}



#endif
