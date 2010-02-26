
#include "managemenu.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QTimer>

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


namespace nota {

ManageMenu::ManageMenu (QWidget * parent)
:QWidget(parent),
 fileDialog (this)
{
  fileUI.setupUi (&fileDialog);
  fileDialog.setWindowTitle (tr("Change Data Location"));
  fileNameAction = new QAction (tr("Data Location"),this);
  menu.addAction (fileNameAction);
  connect (fileNameAction, SIGNAL (triggered()), this, SLOT(ChangeFilename()));

  exportAction = new QAction (tr("Export All"), this);
  menu.addAction (exportAction);
  connect (exportAction, SIGNAL (triggered()), this, SLOT (ExportAll()));
  
  ConnectDialogs ();
}

void
ManageMenu::ConnectDialogs ()
{
  connect (fileUI.saveButton, SIGNAL(clicked()),
           this, SLOT (SaveLoc()));
  connect (fileUI.cancelButton, SIGNAL (clicked()),
           this, SLOT (CancelLoc()));
  #if 0
  connect (exportUI.saveButton, SIGNAL (clicked()),
           this, SLOT (DoExport()));
  connect (exportUI.cancelButton, SIGNAL (clicked()),
           this, SLOT (CancelExport()));
  #endif
}
void
ManageMenu::Exec (QPoint here)
{
 // show ();
  menu.popup (here);
 // hide ();
}

void
ManageMenu::ChangeFilename ()
{
  fileUI.directoryEdit->setText (pConf->Directory());
  fileUI.notesFileEdit->setText (pConf->FileName());
  fileDialog.exec ();
}

void
ManageMenu::ExportAll ()
{
  QString dataHome = QDesktopServices::storageLocation 
                         (QDesktopServices::DataLocation);
  QString copyToHere = QFileDialog::getSaveFileName(this, tr("Save File"),
                            dataHome,
                            tr("All Files (*.*)"));
  if (copyToHere.length() > 0) {
    QFile oldDB (pConf->CompleteDBName());
    QFile newDB (copyToHere);
    if (newDB.exists()) {
      newDB.remove();
    }
    bool copied = oldDB.copy (copyToHere);
    QString status (copyToHere);
    if (copied) {
      status.append (QString (tr(" Written OK")));
    } else {
      status.append (QString (tr(" Copy  failed!")));
    }
    QMessageBox box(this);
    Qt::WindowFlags flags = box.windowFlags();
    flags |= Qt::FramelessWindowHint;
    box.setWindowFlags (flags);
    box.setText (status);
    QTimer::singleShot (30000,&box,SLOT(accept()));
    box.exec ();
  }
}

void
ManageMenu::SaveLoc ()
{
  pConf->SetFileName (fileUI.notesFileEdit->text());
  pConf->SetDirectory (fileUI.directoryEdit->text());
  pConf->Sync ();
  fileDialog.accept ();
  emit SigReload ();
}

void
ManageMenu::CancelLoc ()
{
  fileDialog.reject ();
}

void
ManageMenu::DoExport ()
{
  qDebug () << " do export called";
}

} // namespace

