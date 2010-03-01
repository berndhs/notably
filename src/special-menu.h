#ifndef SPECIAL_MENU_H
#define SPECIAL_MENU_H
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

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "delib-debug.h"
#include "notaconf.h"

namespace nota {

class SpecialMenu : public QWidget {

Q_OBJECT

public:

  SpecialMenu (QWidget *parent);
  
  void SetConf (NotaConf * pC);
  void Init ();
  
  void Exec (QPoint here);


private slots:


  void ScreenShot ();
  void WholeScreenShot ();
  void GrabHtml ();
  void GrabLink ();
  void GrabNoteLink ();
  void PasteNoteLink ();
  
signals:

  void SigShootScreen (const bool whole);
  void SigGrabSelection ();
  void SigGrabLink ();
  void SigCopyNoteLink ();
  void SigPasteNoteLink ();
  
  
private:

  void EmitScreenShot (const QString msg, const bool whole);

  NotaConf  *pConf;
  
  QMenu    menu;

  QAction  *shootAction;
  QAction  *shootAllAction;
  QAction  *htmlAction;
  QAction  *linkAction;
  QAction  *copyLinkAction;
  QAction  *pasteLinkAction;

};


} // namespace

#endif
