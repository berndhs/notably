#include "notesdisplay.h"
#include "deliberate.h"
#include "delib-debug.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QListWidgetItem>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <QMessageBox>
#include <QPixmap>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QCursor>

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

#include <QFile>

using namespace deliberate;

namespace nota {

NotesDisplay::NotesDisplay (QApplication & app)
:pApp(&app),
 didShutdownActions (false),
 pConf(0),
 noteMenu(this),
 editMenu(this),
 specialMenu (this),
 manageMenu (this),
 contentMenu (this),
 helpBox (this),
 noteTagEditor (this),
 noteBookEditor (this),
 htmlExporter (this,db),
 dbManager (db),
 mConName ("nota_dbcon"),
 noLabel (this),
 maxTags (5),
 numTags (0)
{
  setupUi (this);
  SetupMenu ();
  SetupEdit ();
  editBox->SetConf (pConf);
  editBox->SetDB (db);
  editMenu.SetConf (pConf);
  manageMenu.SetConf (pConf);
  manageMenu.SetDB (db);
  contentMenu.SetConf (pConf);
  contentMenu.SetDB (db);
  noteTagEditor.SetDB (db);
  noteBookEditor.SetDB (db);
  noTagPix.load (":img/notag.png");
  noLabel.hide();
  noLabel.setPixmap (noTagPix);
  noLabel.resize (noTagPix.size());
  for (int t=0; t<maxTags; t++) {
    tagLabel[t].setParent(this);
    tagLabel[t].hide();
    tagPix[t].load (":img/notag.png");
  }
  ShowNothing ();
  if (pApp) {
    connect (pApp, SIGNAL (aboutToQuit()), this, SLOT (AboutToQuit()));
  }
  connect (notesIndex, SIGNAL (itemActivated (QListWidgetItem*)),
           this, SLOT (UserPicked (QListWidgetItem*)));
  connect (noteName, SIGNAL (textEdited (const QString &)), 
           this, SLOT (NameChanged(const QString &)));
           
  connect (&debugTimer, SIGNAL (timeout()), this, SLOT (DebugCheck()));
  
  connect (editBox, SIGNAL (NewImage (QString)), 
           this, SLOT (ImageInserted (QString)));
  connect (editBox, SIGNAL (LinkToNote (qint64)),
           this, SLOT (InterNoteLink (qint64)));
  #if DELIBERATE_DEBUG && 0
  debugTimer.start (3000);
  #endif
}

void
NotesDisplay::SetupMenu ()
{
  exitAction = new QAction (tr("&Quit") , this);
  if (!deliberate::IsMaemo ()) {
    menubar->addAction (exitAction);
  }
  noteMenuAction = new QAction (tr("&Note..."), this);
  menubar->addAction (noteMenuAction);
  editAction = new QAction (tr("&Edit..."), this);
  menubar->addAction (editAction);
  specialAction = new QAction (tr("Spec&ial..."), this);
  menubar->addAction (specialAction);
  manageAction = new QAction (tr("M&anage..."), this);
  menubar->addAction (manageAction);
  contentAction = new QAction (tr("Con&tent..."), this);
  menubar->addAction (contentAction);
  helpAction = new QAction (tr("&Help"), this);
  menubar->addAction (helpAction);
  
  
  saveShort = new QShortcut (QKeySequence(tr("Ctrl+S")),this);
  connect (saveShort, SIGNAL(activated()), this, SLOT (SaveCurrent()));
  exitShort = new QShortcut (QKeySequence (tr("Ctrl+Q")), this);
  connect (exitShort, SIGNAL (activated()), this, SLOT (quit()));
  noteMenuShort = new QShortcut (QKeySequence (tr("Ctrl+N")), this);
  connect (noteMenuShort, SIGNAL (activated()), this, SLOT (ShowNoteMenu()));
  editShort = new QShortcut (QKeySequence (tr("Ctrl+E")), this);
  connect (editShort, SIGNAL (activated()), this, SLOT (ScheduleEdit()));
  helpShort = new QShortcut (QKeySequence (tr("Ctrl+H")), this);
  connect (helpShort, SIGNAL (activated()), this, SLOT (Help()));
  manageShort = new QShortcut (QKeySequence (tr("Ctrl+A")), this);
  connect (manageShort, SIGNAL (activated()), this, SLOT (ScheduleManage()));
  contentShort = new QShortcut (QKeySequence (tr("Ctrl+T")), this);
  connect (contentShort, SIGNAL (activated()), this, SLOT (ScheduleContent()));
  specialShort = new QShortcut (QKeySequence (tr("Ctrl+I")), this);
  connect (specialShort, SIGNAL (activated()), this, SLOT (ScheduleSpecial()));
  
  connect (exitAction, SIGNAL (triggered()), this, SLOT (quit())); 
 
  connect (saveButton, SIGNAL (clicked()), this, SLOT (SaveCurrent()));
  
  connect (noteMenuAction, SIGNAL (triggered()), this, SLOT (ShowNoteMenu()));
  connect (editAction, SIGNAL (triggered()), this, SLOT (ScheduleEdit()));
  connect (specialAction, SIGNAL (triggered()), this, SLOT (ScheduleSpecial()));
  connect (manageAction, SIGNAL (triggered()), this, SLOT (ScheduleManage()));
  connect (contentAction, SIGNAL (triggered()), this, SLOT (ScheduleContent()));
  connect (helpAction, SIGNAL (triggered()), this, SLOT (Help()));
  
  connect (backButton, SIGNAL (clicked()), this, SLOT (Back()));
  
  connect (&noteMenu, SIGNAL (SaveNote()), this, SLOT (SaveCurrent()));
  connect (&noteMenu, SIGNAL (PublishNote()), this, SLOT (PublishCurrent()));
  connect (&noteMenu, SIGNAL (DeleteNote()), this, SLOT (DeleteCurrent()));
  connect (&noteMenu, SIGNAL (NewNote()), this, SLOT (NewNote()));
  connect (&noteMenu, SIGNAL (CancelNote()), this, SLOT (ShowNothing()));
  connect (&noteMenu, SIGNAL (ChangeTags()), this, SLOT (DoNoteTags()));
  connect (&noteMenu, SIGNAL (ChangeBooks()), this, SLOT (DoNoteBooks()));
  
  connect (&manageMenu, SIGNAL (SigReload()), this, SLOT (ReloadDB()));
  connect (&manageMenu, SIGNAL (SigExportBook (QString)),
             this, SLOT (ExportBook (QString)));
  
  connect (&contentMenu, SIGNAL (Selected (NoteIdSetType &)),
           this, SLOT (SelectionMade (NoteIdSetType &)));
  connect (&contentMenu, SIGNAL (DoneSelection ()),
           this, SLOT (SelectionMade ()));

  connect (&helpBox, SIGNAL (WantHelp()), this, SLOT (HelpHelp ()));
  connect (&helpBox, SIGNAL (WantLicense()), this, SLOT (LicenseHelp()));
  
  connect (editBox, SIGNAL (textChanged()), this, SLOT (EditBoxChanged()));
}

void
NotesDisplay::SetupEdit ()
{
  connect (&editMenu, SIGNAL (SigFontToggle (const FontProperty)),
            this, SLOT (ToggleFont (const FontProperty)));
  connect (&editMenu, SIGNAL (SigLocalSearch ()), 
           editBox, SLOT (LocalSearch()));
  connect (&editMenu, SIGNAL (SigCopy()), editBox, SLOT (DoCopy()));
  connect (&editMenu, SIGNAL (SigPaste()), editBox, SLOT (DoPaste()));
  connect (&editMenu, SIGNAL (SigUndo()), editBox, SLOT (DoUndo ()));
  connect (&specialMenu , SIGNAL (SigShootScreen (const bool)),
            this, SLOT (ShootScreen (const bool)));
  connect (&specialMenu, SIGNAL (SigGrabSelection ()),
            this, SLOT (GrabHtml()));
  connect (&specialMenu, SIGNAL (SigGrabLink()), 
            this, SLOT (GrabLink ()));
  connect (&specialMenu, SIGNAL (SigCopyNoteLink ()),
            this, SLOT (CopyNoteLink()));
  connect (&specialMenu, SIGNAL (SigPasteNoteLink ()),
            this, SLOT (PasteNoteLink()));
}

void
NotesDisplay::SetApplication (QApplication * pA)
{
  pApp = pA;
}

void
NotesDisplay::SetConf (NotaConf & conf)
{
  pConf = &conf;
  editBox->SetConf (pConf);
  editMenu.SetConf (pConf);
  specialMenu.SetConf (pConf);
  manageMenu.SetConf (pConf);
  contentMenu.SetConf (pConf);
  dbManager.SetConf (pConf);
}

void
NotesDisplay::Start ()
{
  editMenu.Init ();
  specialMenu.Init ();
  if (Settings().contains("size")) {
    QSize defaultSize = size();
    QSize newsize = Settings().value ("size", defaultSize).toSize();
    resize (newsize);
  }
  FillNotesList (notesIndex);
  if (pConf) {
    QDir::setCurrent (pConf->Directory());
  }
}

void
NotesDisplay::resizeEvent (QResizeEvent * event)
{
  QSize newsize = event->size();
  Settings().setValue ("size",newsize);
}

void
NotesDisplay::ReloadDB ()
{
  CloseDB ();
  if (!DBExists()) {
    dbManager.MakeTables (mConName);
  }
  OpenDB ();
  FillNotesList (notesIndex);
}

void
NotesDisplay::DoNoteTags ()
{
  noteTagEditor.ManageTags (currentId, currentName);
  ListTags (currentId);
}

void
NotesDisplay::DoNoteBooks ()
{
  noteBookEditor.ManageBooks (currentId, currentName);
}

void
NotesDisplay::ShutdownClean ()
{
  if (!didShutdownActions) {
    CloseDB ();
    Settings().sync();
    didShutdownActions = true;
  }
}


void
NotesDisplay::quit ()
{
  ShutdownClean();
  if (pApp) {
    pApp->quit();
  }
}

void
NotesDisplay::AboutToQuit ()
{
  ShutdownClean();
}

void
NotesDisplay::dropEvent (QDropEvent *event)
{
  qDebug () << "drop event " << event;
  
}

void
NotesDisplay::GrabHtml ()
{
  editBox->GrabHtml ();
}

void
NotesDisplay::GrabLink ()
{
  editBox->GrabLink ();
}

void
NotesDisplay::CopyNoteLink ()
{
  if (showingNote) {
    QString idstring = QString::number (currentId);
    noteLinkCopy = QString ("notably://%1").arg(idstring);
  } else {
     // complain that there is no current saved note
    QMessageBox box(this);
    Qt::WindowFlags flags = box.windowFlags();
    flags |= Qt::FramelessWindowHint;
    box.setWindowFlags (flags);
  
    box.setText (tr("This Note has no identifier, Save it first!"));
    int delay = 10000;
    QTimer::singleShot (delay, &box, SLOT (accept()));
    box.exec ();
  }
}

void
NotesDisplay::PasteNoteLink ()
{
  editBox->PasteLink (noteLinkCopy);
}

void
NotesDisplay::ToggleFont (const FontProperty prop)
{
  if (prop <= FP_none || prop >= FP_max) {
    return;
  }
  bool newVal = !fontProperty[prop];
  fontProperty[prop] = newVal;
  switch (prop) {
  case FP_bold:
    editBox->setFontWeight (newVal ? QFont::Bold : QFont::Normal);
    break;
  case FP_italic:
    editBox->setFontItalic (newVal);
    break;
  case FP_underline:
    editBox->setFontUnderline (newVal);
    break;
  default:
    break;
  }
}

void
NotesDisplay::Help ()
{
  helpBox.setText (deliberate::ProgramVersion::Version());
  helpBox.exec ();
}

void
NotesDisplay::EditBoxChanged ()
{
  isChanged = true;
}

void
NotesDisplay::HelpHelp ()
{
  helpBrowser.ShowPage ("qrc:qrcfiles/userman.html");
}

void
NotesDisplay::LicenseHelp ()
{
  helpBrowser.ShowPage ("qrc:qrcfiles/LICENSE.txt");
}

void
NotesDisplay::ShowNoteMenu ()
{
  QTimer::singleShot (50, this, SLOT (ExecNoteMenu()));
}

void
NotesDisplay::ScheduleEdit ()
{
  QTimer::singleShot (50, this, SLOT (ExecEditMenu()));
}

void
NotesDisplay::ScheduleManage ()
{
  QTimer::singleShot (50, this, SLOT (ShowManage()));
}

void
NotesDisplay::ScheduleSpecial ()
{
  QTimer::singleShot (50, this, SLOT (DoSpecial()));
}

void
NotesDisplay::DoSpecial ()
{
  specialMenu.Exec (editBox->mapToGlobal (QPoint(0,0)));
}

void
NotesDisplay::ScheduleContent ()
{
  QTimer::singleShot (50, this, SLOT (DoContent()));
}

void
NotesDisplay::ExecEditMenu ()
{
  editMenu.Exec (editBox->mapToGlobal(QPoint(0,0)));
}

void
NotesDisplay::ExecNoteMenu ()
{
  noteMenu.Exec (editBox->mapToGlobal(QPoint(0,0)));
}

void
NotesDisplay::ShowManage ()
{
  manageMenu.Exec (editBox->mapToGlobal (QPoint(0,0)));
}

void
NotesDisplay::DoContent ()
{
  contentMenu.Exec (editBox->mapToGlobal (QPoint(0,0)));
}

void
NotesDisplay::UserPicked (QListWidgetItem *item)
{
  if (item) {
    qint64 id = item->data(Qt::UserRole).toLongLong();
    QString name = item->text();
    ShowNote (item,id,name);
  }
}

void
NotesDisplay::DeleteFromDB (const qint64 id)
{
  OpenDB ();
  
  QStringList tables;
  tables << "notes" << "tagrefs" << "imagerefs";
  QStringList::iterator tableit;
  QString qryPattern ("delete from %1 where noteid = %2");
  QString qryStr;
  QSqlQuery qry (db);
  bool ok;
  for (tableit = tables.begin(); tableit != tables.end(); tableit++)
  {
    qryStr = qryPattern.arg(*tableit).arg (QString::number(id));
    ok = qry.exec (qryStr);
  }
}

void
NotesDisplay::DeleteCurrent ()
{
  if (showingNote) {
    DeleteFromDB (currentId);
    if (curItem) {
      int row = notesIndex->row (curItem);
      QListWidgetItem * deadItem = notesIndex->takeItem (row);
      if (deadItem) {
        delete deadItem;
        curItem = 0;
      }
    }
  }
  ShowNothing();
}

void
NotesDisplay::ShowNote (QListWidgetItem *item,
                        const qint64 id, 
                        const QString & name)
{
  QString textbody;
  QString qstr ("select usergivenid, notetext from 'notes' where noteid ='");
  qstr.append (QString::number(id));
  qstr.append ("'");
  OpenDB ();
  QSqlQuery query (db);
  bool ok = query.exec (qstr);
  if (ok && query.next()) {
    int textindex = query.record().indexOf("notetext");
    textbody = query.value(textindex).toString ();
    noteName->setText (name);
    editBox->setHtml (textbody);
    showingNote = true;
    curItem = item;
    currentId = id;
    currentName = name;
    newName = currentName;
    nameChanged = false;
    isNew = false;
    ListTags (currentId);
    isChanged = false;
  }
}

void
NotesDisplay::InterNoteLink (qint64 nextnote, bool forward)
{
  QString qryPattern 
         ("select usergivenid from 'notes' where noteid=%1");
  QString qryStr = qryPattern.arg(QString::number(nextnote));
  QSqlQuery query (db);
  bool ok = query.exec (qryStr);
  if (ok && query.next()) {
    if (forward) {
      idStack.push (currentId);
    }
    QString name = query.value(0).toString();
    if (isChanged) {
      SaveCurrent ();
    }
    ShowNote (0,nextnote,name);
  }
}

void
NotesDisplay::Back ()
{
  if (!idStack.isEmpty()) {
    qint64 newid = idStack.pop();
    InterNoteLink (newid, false);
  }
}


void
NotesDisplay::GetTagPix (const QString tagname, int t)
{
  OpenDB ();
  QString qryPattern ("select icon from tags where tagname = '%1'");
  QString qryStr = qryPattern.arg (tagname);
  QSqlQuery query (db);
  query.exec (qryStr);
  if (query.next()) {
    QByteArray bytes = query.value(0).toByteArray();
    QPixmap localpix;
    localpix.loadFromData (bytes);
    tagPix[t] = localpix;
  } else {
    tagPix[t] = noTagPix;
  }
}


void
NotesDisplay::ListTags (const qint64 noteid)
{
  QStringList tagnames;
  noteTagEditor.GetTagnames (noteid, tagnames);
  QPoint offset (3,4);
  QPoint topleft (centralwidget->pos());
  topleft.rx() += noteName->pos().x();
  topleft += offset;
  int ntags = tagnames.size();
  if (tagnames.size() < 1) {
    noLabel.hide();
    tagLabel[0].setPixmap (noTagPix);
    tagLabel[0].resize (noTagPix.size());
    tagLabel[0].move (topleft);
    tagLabel[0].show ();
    tagLabel[0].raise ();
    tagLabel[0].clearFocus();
    numTags = 1;
  } else {
    noLabel.hide ();
    if (ntags > maxTags) { ntags = maxTags; }
    QPoint  here = topleft;
    for (int t=0; t<ntags; t++) {
      GetTagPix (tagnames[t], t);
      tagLabel[t].setPixmap (tagPix[t]);
      tagLabel[t].resize (tagPix[t].size());
      tagLabel[t].move (here);
      tagLabel[t].show ();
      tagLabel[t].raise ();
      tagLabel[t].clearFocus ();
      here.rx() += tagLabel[t].width() +2;
    }
    numTags = ntags;
  }
  for (int mt=numTags; mt<maxTags; mt++) {
    tagLabel[mt].hide();
  }
}

void
NotesDisplay::ShowNothing ()
{
  isNew = false;
  currentId = 0;
  currentName = "";
  newName = "";
  nameChanged = false;
  isChanged = false;
  noteName ->setText ("");
  editBox->setHtml ("");
  noLabel.hide();
  numTags = 0;
  curItem = 0;
  showingNote = false;
  fontProperty[FP_bold] = false;
  fontProperty[FP_italic] = false;
  fontProperty[FP_underline] = false;
  for (int mt=0; mt<maxTags; mt++) {
    tagLabel[mt].hide();
  }
}

void
NotesDisplay::MakeNew (qint64 & id, QString &name)
{
  QDateTime now = QDateTime::currentDateTime ();
  id = now.toTime_t();
  name = now.toString ("yyyy-MM-dd-hh-mm-ss-Note");
}

void
NotesDisplay::NewNote ()
{
  MakeNew (currentId, currentName);
  newName = currentName;
  nameChanged = false;
  curItem = 0;
  isNew = true;
  noteName->setText (currentName);
  editBox->setHtml (QString("New Note"));
  showingNote = true;
  ListTags (currentId);
}


void
NotesDisplay::NameChanged (const QString & name)
{
  if (currentName != name) {
    nameChanged = true;
    newName = name;
  }
}

void
NotesDisplay::ImageInserted (QString imgname)
{
  QString qryStr ("insert or replace into imagerefs ( noteid, imageref) "
                  " values (?, ?)");
  QSqlQuery query (db);
  query.prepare (qryStr);
  QVariant  idval;
  QVariant  imgnameval;
  idval.setValue (currentId);
  query.bindValue (0,idval);
  imgnameval.setValue (imgname);
  query.bindValue (1, imgnameval);
  query.exec ();
}

void
NotesDisplay::SaveCurrent ()
{
  if (!DBExists()) {
    dbManager.MakeTables (mConName);
  }
  OpenDB ();
  if (!showingNote) {
    MakeNew (currentId, currentName);
    isNew = true;
  }
  if (nameChanged) {
    currentName = newName;
  }
  WriteNote (currentId, currentName, editBox->toHtml());
  if (isNew) {
    curItem = ListThisNote (notesIndex, currentId, currentName);
    noteName->setText (currentName);
  } else if (nameChanged) {
    curItem->setText (currentName);
  }
  isNew = false;
  nameChanged = false;
  showingNote = true;
}

void
NotesDisplay::PublishCurrent ()
{
  QString wholeName (pConf->Directory() + QDir::separator() + noteName->text() + ".html");
  QFileInfo defaultInfo (wholeName);
  QString copyToHere = QFileDialog::getSaveFileName(this, tr("Save File"),
                            wholeName,
                            tr("All Files (*.*)"));
  if (copyToHere.length() > 0) {
    QFile pageFile (copyToHere);
    pageFile.open (QFile::WriteOnly);
    qint64 nbytes = pageFile.write (editBox->toHtml().toLocal8Bit());
    pageFile.close();
    QFileInfo destInfo (copyToHere);
    QMessageBox report (this);
    Qt::WindowFlags flags = report.windowFlags();
    flags |= Qt::FramelessWindowHint;
    report.setWindowFlags (flags);
    QString savedMessage;
    if (nbytes > 0) {
      savedMessage = QString (tr("Note exported as %1"))
                              .arg (copyToHere);  
      if (destInfo.path() != defaultInfo.path()) {
        CopyPageImages (currentId, defaultInfo.path(), destInfo.path ());
      }
    } else {
      savedMessage = QString (tr("Could not write %1 !"))
                              .arg (copyToHere);
    }
    report.setText (savedMessage);
    QTimer::singleShot (20000, &report, SLOT(accept()));
    report.exec ();
  }
}

void
NotesDisplay::CopyPageImages (const qint64 noteid,
                              const QString srcdir,
                              const QString destdir)
{
  QString selectPattern ("select imageref from imagerefs where noteid = %1");
  QString select = selectPattern.arg(QString::number(noteid));
  QSqlQuery query (db);
  QString imgname;
  QString srcfile;
  QString destfile;
  QChar    sep = QDir::separator();
  bool ok = query.exec (select);
  QDir dest (destdir);
  if (!dest.exists()) {
    dest.mkpath (destdir);
  }
  while (query.next()) {
    imgname = query.value(0).toString();
    srcfile = srcdir + sep + imgname;
    destfile = destdir + sep + imgname;
    QFile oldFile (srcfile);
    QFile newFile (destfile);
    if (newFile.exists()) {
      newFile.remove();
    }
    ok = oldFile.copy (destfile);
  }
  
}

void
NotesDisplay::ReportText ()
{
  StdOut() << editBox->toHtml ();
}

void
NotesDisplay::OpenDB ()
{
  if (db.isOpen()) {
    return;
  }
  db = QSqlDatabase::addDatabase ("QSQLITE",mConName);
  db.setDatabaseName (pConf->CompleteDBName());
  db.open ();
}

void
NotesDisplay::CloseDB ()
{
  db.close();
  QSqlDatabase::removeDatabase (mConName);
}

bool
NotesDisplay::DBExists ()
{
  QString name = pConf->CompleteDBName();
  QFileInfo inf (name);
  if (!inf.exists()) {
    return false;
  }
  if (!inf.isWritable()) {
    return false;
  }
  QString tableQuery ("select name from sqlite_master where type='table'");
  QSqlQuery askTables (db);
  bool ok = askTables.exec (tableQuery);
  int numtables (0);
  if (!ok) {
    return false;
  }
  while (askTables.next()) {
    numtables ++;
  }
  if (numtables < 1) {
    return false;
  }
  return true;
}


void
NotesDisplay::FillNotesList (  QListWidget * notesIndex)
{
  OpenDB ();
  QString  notesQuery ("select noteid, usergivenid from 'notes' where 1");
  QSqlQuery ListQuery (db);
  ListQuery.prepare (notesQuery);
  ListQuery.exec ();
  int idField = ListQuery.record().indexOf ("noteid");
  int nameField = ListQuery.record().indexOf ("usergivenid");
  QString name;
  qint64  id;
  notesIndex->clear ();
  while (ListQuery.next()) {
    id = ListQuery.value(idField).toLongLong();
    name = ListQuery.value(nameField).toString();
    ListThisNote (notesIndex, id, name);
  }
}

void
NotesDisplay::FillNotesList ( QListWidget * notesIndex, 
                              NoteIdSetType & idset)
{
  notesIndex->clear ();
  QString qryPattern ("select  usergivenid from 'notes' where noteid=%1");
  NoteIdSetType::iterator idit;
  QSqlQuery query (db);
  QString name;
  QString qryStr;
  for (idit = idset.begin(); idit != idset.end(); idit++) {
    qryStr = qryPattern.arg(*idit);
    query.exec (qryStr);
    if (query.next()) {
      name = query.value(0).toString();
      ListThisNote (notesIndex, *idit, name);
    }
  }
}

void
NotesDisplay::SelectionMade (NoteIdSetType & idset)
{
  ShowNothing();
  FillNotesList (notesIndex, idset);
}

void
NotesDisplay::SelectionMade ()
{
  FillNotesList (notesIndex, contentMenu.SelectedNotes());
}

QListWidgetItem *
NotesDisplay::ListThisNote (      QListWidget *notesIndex,
                            const qint64  id,
                            const QString  &name)
{
  QListWidgetItem *item(0);
  item = new QListWidgetItem (notesIndex);
  item->setText (name);
  item->setData (Qt::UserRole, QVariant(id));
  notesIndex->addItem (item);
  return item;
}

void
NotesDisplay::WriteNote (const qint64 id,
                       const QString & name,
                       const QString & text)
{
  QSqlQuery InsertQuery (db);
  QString queryPattern ("insert or replace into 'notes' (noteid, usergivenid, notetext)");
  queryPattern.append (" VALUES (?,?,?)");
  InsertQuery.prepare (queryPattern);
  QVariant v[3];
  v[0].setValue (id);
  v[1].setValue (name);
  v[2].setValue (text);
  for (int i=0; i<3; i++) {
    InsertQuery.bindValue (i,v[i]);
  }
  bool worked = InsertQuery.exec ();
  QString result;
  if (worked) {
    QString pat (tr("saved as \"%1\""));
    result = QString(pat.arg(name));
  } else {
    result = QString(tr("save failed!"));
  }
  QMessageBox box(this);
  Qt::WindowFlags flags = box.windowFlags();
  flags |= Qt::FramelessWindowHint;
  box.setWindowFlags (flags);
  
  box.setText (result);
  int delay = (worked ? 1500 : 15000);
  QTimer::singleShot (delay, &box, SLOT (accept()));
  box.exec ();
}

void
NotesDisplay::ShootScreen (const bool whole)
{
  if (whole) {
    hide();
  } else {
    this->grabMouse (Qt::CrossCursor);
  };
  QDesktopWidget *desk = QApplication::desktop();
  QRect dimensions = desk->screenGeometry (this);
  corner1 = dimensions.topLeft();
  corner2 = dimensions.bottomRight ();
  QTimer::singleShot (5000, this, SLOT (DoShootScreen()));
}


void
NotesDisplay::DoShootScreen ()
{
  this->releaseMouse ();
  int x1 = corner1.x ();
  int y1 = corner1.y ();
  int x2 = corner2.x ();
  int y2 = corner2.y ();
  int swap;
  if (x2 < x1) {
    swap = x1;
    x1 = x2;
    x2 = swap;
  }
  if (y2 < y1) {
    swap = y1;
    y1 = y2;
    y2 = swap;
  }
  QPixmap pixBuffer = QPixmap::grabWindow (QApplication::desktop()->winId(),
                              x1,y1, x2-x1, y2-y1 );
  QImage img = pixBuffer.toImage();
  editBox->InsertImage (img);
  show ();
}

void
NotesDisplay::mousePressEvent (QMouseEvent *event)
{
  corner1 = event->globalPos();
}

void
NotesDisplay::mouseReleaseEvent (QMouseEvent *event)
{
  corner2 = event->globalPos();
}

void
NotesDisplay::ExportBook (QString bookname)
{
  htmlExporter.ExportBook (bookname);
}

void
NotesDisplay::DebugCheck ()
{
#ifndef _WIN32
  qDebug () << " debug timer check " << time (0);
  qDebug () << " editbox modified " << isChanged;
  qDebug () << " isNew " << isNew;
  qDebug () << " nameChanged " << nameChanged;
  qDebug () << " showingNote " << showingNote;
  qDebug () << " currentId " << currentId;
  qDebug () << " currentName " << currentName;
  qDebug () << " newName " << newName;
#endif
}

}
