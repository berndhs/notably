
#include "managemenu.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QTimer>
#include "deliberate.h"

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

using namespace deliberate;

namespace nota {

ManageMenu::ManageMenu (QWidget * parent)
:QWidget(parent),
 fileDialog (this),
 bookEditor (this),
 tagEditor (this),
 bookPicker (this)
{
  fileUI.setupUi (&fileDialog);
  fileDialog.setWindowTitle (tr("Database Location"));

  exportAction = new QAction (tr("Export All"), this);
  menu.addAction (exportAction);
  bookExSqlAction = new QAction (tr("Export Book as Sql"), this);
  menu.addAction (bookExSqlAction);
  
  htmlAction = new QAction (tr("Export Book as HTML"), this);
  menu.addAction (htmlAction);
  if (!IsFingerInterface()) {
    menu.addSeparator ();
  }
  mergeAction = new QAction (tr("Import Notes..."), this);
  menu.addAction (mergeAction);
  if (!IsFingerInterface()) {
    menu.addSeparator ();
  }
  bookAction = new QAction (tr("Book Editor"), this);
  menu.addAction (bookAction);
  tagAction = new QAction (tr("Tag Editor"), this);
  menu.addAction (tagAction);
  if (!IsFingerInterface()) {
    menu.addSeparator ();
  }
  fileNameAction = new QAction (tr("Preferences..."),this);
  menu.addAction (fileNameAction);
  
  connect (exportAction, SIGNAL (triggered()), this, SLOT (ExportAll()));
  connect (htmlAction, SIGNAL (triggered()), this, SLOT (ExportBook()));
  connect (bookExSqlAction, SIGNAL (triggered()), this, SLOT (ExportBookSql()));
  connect (bookAction, SIGNAL (triggered()), this, SLOT (EditBooks()));
  connect (tagAction, SIGNAL (triggered()), this, SLOT (EditTags()));
  connect (fileNameAction, SIGNAL (triggered()), this, SLOT(ChangeFilename()));
  connect (mergeAction, SIGNAL (triggered()), this, SLOT (MergeOtherDB()));
  
  ConnectDialogs ();
}

void
ManageMenu::SetDB (QSqlDatabase & db)
{
  bookEditor.SetDB (db);
  tagEditor.SetDB (db);
  bookPicker.SetDB (db);
}

void
ManageMenu::ConnectDialogs ()
{
  connect (fileUI.saveButton, SIGNAL(clicked()),
           this, SLOT (SaveLoc()));
  connect (fileUI.cancelButton, SIGNAL (clicked()),
           this, SLOT (CancelLoc()));
  connect (fileUI.changeButton, SIGNAL (clicked()),
           this, SLOT (GetNewFilename()));
}

void
ManageMenu::ImportBusy (bool busy)
{
  if (mergeAction) {
    mergeAction->setEnabled (!busy);
  }
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
ManageMenu::GetNewFilename ()
{
  QString newName ;
  QString defaultTarget = fileUI.directoryEdit->text()
                         + QDir::separator()
                         + fileUI.notesFileEdit->text();
                    
  QFileDialog askThem (this, tr("Database File"),
                       defaultTarget,
                       tr("SQLite Files (*.sql);; All Files (*.*)"));
  askThem.setAcceptMode (QFileDialog::AcceptSave);
  askThem.setConfirmOverwrite (false);
  askThem.setFileMode (QFileDialog::AnyFile);
  int wantit = askThem.exec();        
  QStringList results = askThem.selectedFiles();
  if (wantit > 0 && !results.isEmpty()) {
    newName = results.first();
    QFileInfo info (newName);
    QString dir = info.path();
    QString file = info.fileName();
    fileUI.directoryEdit->setText (dir);
    fileUI.notesFileEdit->setText (file);
  }
  
}

void
ManageMenu::MergeOtherDB ()
{
  QString userHome = QDesktopServices::storageLocation 
                         (QDesktopServices::HomeLocation);
  QString mergeFromHere = QFileDialog::getOpenFileName(this, tr("Open File"),
                            userHome,
                            tr("SQLite Files (*.sql);; All Files (*.*)"));
  if (mergeFromHere.length() > 0 ) {
    emit SigMerge (mergeFromHere);
  }
}

void
ManageMenu::ExportAll ()
{
  QString defaultTarget = QDesktopServices::storageLocation 
                         (QDesktopServices::DataLocation)
                         + QDir::separator()
                         + tr("nota-export.sql");
  QString copyToHere ;
                    
  QFileDialog askThem (this, tr("Save File"),
                       defaultTarget,
                       tr("SQLite Files (*.sql);; All Files (*.*)"));
  askThem.setAcceptMode (QFileDialog::AcceptSave);
  askThem.setConfirmOverwrite (true);
  askThem.setFileMode (QFileDialog::AnyFile);
  int wantit = askThem.exec();        
  QStringList results = askThem.selectedFiles();
  if (wantit > 0 && !results.isEmpty()) {
    copyToHere = results.first();
  }
                            
  if (copyToHere.length() > 0) {
    QFile oldDB (pConf->CompleteDBName());
    QFile newDB (copyToHere);
    if (newDB.exists()) {
      newDB.remove();
    }
    bool copied = oldDB.copy (copyToHere);
    QString status (copyToHere);
    if (copied) {
      QFileInfo info (copyToHere);
      emit (SigExportImages (info.path()));
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
ManageMenu::ExportBook ()
{ 
  if (ExportHtml::IsImplemented()) {
    int wantExport = bookPicker.Exec ();
    if (wantExport) {
      emit SigExportBook (bookPicker.TitleSelected());
    }
  } else {
    QMessageBox box (this);
    Qt::WindowFlags flags = box.windowFlags();
    flags |= Qt::FramelessWindowHint;
    box.setWindowFlags (flags);
    box.setText (tr("Not Implemented for Qt Version less than 4.6.1"));
    QTimer::singleShot (10000, &box, SLOT (accept()));
    box.exec ();
  }
}

void
ManageMenu::ExportBookSql ()
{
  int wantExport = bookPicker.Exec ();
  if (wantExport) {
    QString bookname = bookPicker.TitleSelected();
    QString defaultTarget = QDesktopServices::storageLocation 
                         (QDesktopServices::HomeLocation)
                         + QDir::separator()
                         + bookname + tr(".sql");
    QFileDialog askThem (this, tr("Save File"),
                         defaultTarget,
                         tr("SQLite Files (*.sql);; All Files (*.*)"));
    askThem.setAcceptMode (QFileDialog::AcceptSave);
    askThem.setConfirmOverwrite (true);
    askThem.setFileMode (QFileDialog::AnyFile);
    int wantit = askThem.exec();       
    QStringList results = askThem.selectedFiles();
    QString     mergeToHere;
    if (wantit > 0 && !results.isEmpty()) {
      mergeToHere = results.first();
    }
    if (mergeToHere.length() > 0 ) {
      emit SigExportBookSql (bookname, mergeToHere);
    }
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
ManageMenu::EditBooks ()
{
  bookEditor.Exec ();
}

void
ManageMenu::EditTags ()
{
  tagEditor.Exec ();
}

} // namespace


