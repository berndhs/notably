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

#include "deliberate.h"
#include <QTextEdit>
#include <QDropEvent>
#include <QImage>
#include <QSqlDatabase>
#include "notaconf.h"
#include "ui_getstring.h"

namespace nota {

class EditBox : public QTextEdit {
Q_OBJECT
public:

  EditBox (QWidget *parent);
  
  void SetConf (NotaConf *pc) { pConf = pc; }
  void SetDB   (QSqlDatabase &db) { pDB = &db; }

  void dropEvent (QDropEvent * event);
  void dragEnterEvent (QDragEnterEvent *event);
  void InsertImage (QImage & img);
  QString BoxText ();
  
  void insertFromMimeData ( const QMimeData * source );
  void mousePressEvent ( QMouseEvent * event );
  
  void GrabHtml ();
  void GrabLink ();
  
  void PasteLink (QString urlstring);
  
public slots:

  void LocalSearch ();
  void DoSearch ();
  void DoCopy ();
  void DoPaste ();
  void DoUndo ();

signals:

  void NewImage (QString filename);
  void LinkToNote (qint64 noteid);
  
private:

  void IsolateUrls (QDropEvent *event);
  QString ImgFilename (QImage & img);
  QString FindUsergivenId (qint64 noteid);
  void SetupSearchbox ();
  
  void  InsertLink (QTextCursor & curse, QString text);
  
  NotaConf *pConf;
  QSqlDatabase  *pDB;
  
  QDialog searchBox;
  Ui_GetStringDialog   searchUi;
  
};

}



#endif
