#include "notesdisplay.h"
#include "delib-debug.h"
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

NotesDisplay::NotesDisplay ()
:pApp(0),
 pConf(0),
 noteMenu(this),
 editMenu(this),
 helpBox (this),
 mConName ("nota_dbcon"),
 noLabel (this),
 maxTags (5),
 numTags (0)
{
  setupUi (this);
  SetupMenu ();
  SetupEdit ();
  editBox->SetConf (pConf);
  editMenu.SetConf (pConf);
  noTagPix.load (":img/notag.jpg");
  noLabel.hide();
  noLabel.setPixmap (noTagPix);
  noLabel.resize (noTagPix.size());
  for (int t=0; t<maxTags; t++) {
    tagLabel[t].setParent(this);
    tagLabel[t].hide();
  }
  ShowNothing ();
  connect (notesIndex, SIGNAL (itemActivated (QListWidgetItem*)),
           this, SLOT (UserPicked (QListWidgetItem*)));
  connect (noteName, SIGNAL (textEdited (const QString &)), 
           this, SLOT (NameChanged(const QString &)));
           
  connect (&debugTimer, SIGNAL (timeout()), this, SLOT (DebugCheck()));
// debugTimer.start (1000);
}

void
NotesDisplay::SetupMenu ()
{
  exitAction = new QAction (tr("&Quit") , this);
  menubar->addAction (exitAction);
  saveAction = new QAction (tr("&Save Note"), this);
  menubar->addAction (saveAction);
  noteMenuAction = new QAction (tr("&Note..."), this);
  menubar->addAction (noteMenuAction);
  editAction = new QAction (tr("&Edit..."), this);
  menubar->addAction (editAction);
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
  
  connect (exitAction, SIGNAL (triggered()), this, SLOT (quit()));
 
  connect (saveAction, SIGNAL (triggered()), this, SLOT (SaveCurrent()));
  
  connect (noteMenuAction, SIGNAL (triggered()), this, SLOT (ShowNoteMenu()));
  connect (editAction, SIGNAL (triggered()), this, SLOT (ScheduleEdit()));
  connect (helpAction, SIGNAL (triggered()), this, SLOT (Help()));
  
  connect (&noteMenu, SIGNAL (SaveNote()), this, SLOT (SaveCurrent()));
  connect (&noteMenu, SIGNAL (PublishNote()), this, SLOT (PublishCurrent()));
  connect (&noteMenu, SIGNAL (DeleteNote()), this, SLOT (DeleteCurrent()));
  connect (&noteMenu, SIGNAL (NewNote()), this, SLOT (NewNote()));
  connect (&noteMenu, SIGNAL (CancelNote()), this, SLOT (ShowNothing()));
  
  connect (&helpBox, SIGNAL (WantHelp()), this, SLOT (HelpHelp ()));
  connect (&helpBox, SIGNAL (WantLicense()), this, SLOT (LicenseHelp()));
}

void
NotesDisplay::SetupEdit ()
{
  connect (&editMenu, SIGNAL (SigFontToggle (const FontProperty)),
            this, SLOT (ToggleFont (const FontProperty)));
  connect (&editMenu , SIGNAL (SigShootScreen (const bool)),
            this, SLOT (ShootScreen (const bool)));
  connect (&editMenu, SIGNAL (SigGrabSelection ()),
            this, SLOT (GrabHtml()));
  connect (&editMenu, SIGNAL (SigGrabLink()), 
            this, SLOT (GrabLink ()));
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
}

void
NotesDisplay::Start ()
{
  editMenu.Init ();
  FillNotesList (notesIndex);
}

void
NotesDisplay::quit ()
{
  if (pApp) {
    pApp->quit();
  }
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
NotesDisplay::HelpHelp ()
{
  helpBrowser.ShowPage (":qrcfiles/userman.html");
}

void
NotesDisplay::LicenseHelp ()
{
  helpBrowser.ShowPage (":qrcfiles/LICENSE.txt");
}

void
NotesDisplay::ShowNoteMenu ()
{
  actionTimer.setSingleShot (true);
  connect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecNoteMenu()));
  actionTimer.start (100);
}

void
NotesDisplay::ScheduleEdit ()
{
  actionTimer.setSingleShot (true);
  connect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecEditMenu()));
  actionTimer.start (100);
}

void
NotesDisplay::ExecEditMenu ()
{
  disconnect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecEditMenu()));
  editMenu.Exec (editBox->mapToGlobal(QPoint(0,0)));
}

void
NotesDisplay::ExecNoteMenu ()
{
  disconnect (&actionTimer, SIGNAL (timeout()), this, SLOT (ExecNoteMenu()));
  noteMenu.Exec (editBox->mapToGlobal(QPoint(0,0)));
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
  QString delStr ("delete from notes where noteid ='");
  delStr.append (QString::number(id));
  delStr.append ("'");
  QSqlQuery qry (db);
  qry.exec (delStr);
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
  if (ok) {
    if (query.next()) {
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
      QPoint topleft (centralwidget->pos());
      topleft.rx() += noteName->pos().x();
      ListTags (topleft, currentId);
    }
  }
  
}


void
NotesDisplay::GetTagPix (const QString tagname, QPixmap & pix)
{
  OpenDB ();
  QString qryPattern ("select icon from tags where tagname = '%1'");
  QString qryStr = qryPattern.arg (tagname);
  QSqlQuery query (db);
  query.exec (qryStr);
  if (query.next()) {
    int index = query.record().indexOf ("icon");
    QByteArray bytes = query.value(index).toByteArray();
    pix.loadFromData (bytes);
  } else {
    pix = noTagPix;
  }
}
void
NotesDisplay::ListTags (const QPoint topleft, const qint64 noteid)
{
  QStringList tagnames;
  GetTagnames (noteid, tagnames);
  int ntags = tagnames.size();
  if (tagnames.size() < 1) {
    QPoint tagPos (topleft);
    noLabel.move (tagPos);
    noLabel.show ();
    noLabel.raise ();
    noLabel.clearFocus ();
    numTags = 1;
  } else {
    noLabel.hide ();
    if (ntags > maxTags) { ntags = maxTags; }
    QPoint  here = topleft;
    for (int t=0; t<ntags; t++) {
      GetTagPix (tagnames[t], tagPix[t]);
      tagLabel[t].setPixmap (tagPix[t]);
      tagLabel[t].resize (tagPix[t].size());
      tagLabel[t].move (here);
      tagLabel[t].show ();
      tagLabel[t].raise ();
      tagLabel[t].clearFocus ();
      here.rx() += tagLabel[t].width();
    }
    numTags = ntags;
  }
  for (int mt=ntags; mt<maxTags; mt++) {
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
  noteName ->setText ("");
  editBox->setHtml ("");
  noLabel.hide();
  numTags = 0;
  curItem = 0;
  showingNote = false;
  fontProperty[FP_bold] = false;
  fontProperty[FP_italic] = false;
  fontProperty[FP_underline] = false;
}

void
NotesDisplay::MakeNew (qint64 & id, QString &name)
{
  QDateTime now = QDateTime::currentDateTime ();
  id = now.toTime_t();
  name = now.toString ("yyyy-MM-dd-hh:mm:ss-Note");
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
NotesDisplay::SaveCurrent ()
{
  if (!DBExists()) {
    MakeTables ();
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
}

void
NotesDisplay::PublishCurrent ()
{
  QString wholeName (pConf->Directory() + "/" + noteName->text() + ".html");
  QFile pageFile (wholeName);
  pageFile.open (QFile::WriteOnly);
  pageFile.write (editBox->toHtml().toLocal8Bit());
  pageFile.close();
  QMessageBox report;
  QString savedMessage = QString (tr("Note saved as %1"))
                            .arg (wholeName);
  report.setText (savedMessage);
  QTimer::singleShot (20000, &report, SLOT(accept()));
  report.exec ();
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
    return true;
  }
  return false;
}

void
NotesDisplay::MakeTable (QString table)
{
  OpenDB ();
  QString filename = QString (":qrcfiles/schema-") + table 
                      + QString (".sql");
  QFile schemafile (filename);
  schemafile.open (QFile::ReadOnly);
  QByteArray createcommands = schemafile.readAll ();
  schemafile.close ();
  QString querytext (createcommands);
  QSqlQuery qry (db);
  qry.prepare (querytext);
  qry.exec ();
}

void 
NotesDisplay::MakeTables ()
{
  MakeTable ("notes");
  MakeTable ("tags");
  MakeTable ("tagrefs");
  MakeTable ("identndx");
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
  while (ListQuery.next()) {
    id = ListQuery.value(idField).toLongLong();
    name = ListQuery.value(nameField).toString();
    ListThisNote (notesIndex, id, name);
  }
}

void
NotesDisplay::GetTagnames ( const qint64 noteid, QStringList & names)
{
  OpenDB ();
  names.clear ();
  QString tagsPattern ("select tagname from tagrefs where noteid = '%1'");
  QString tagsQuery = tagsPattern.arg(QString::number(noteid));
  QSqlQuery query (db);
  query.exec (tagsQuery);
  int tagField = query.record().indexOf ("tagname");
  QString tagname;
  while (query.next()) {
    names.append (query.value(tagField).toString());
  }
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
  InsertQuery.exec ();
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
NotesDisplay::DebugCheck ()
{
#ifndef _WIN32
  qDebug () << " debug timer check " << time (0);
#endif
}

}
