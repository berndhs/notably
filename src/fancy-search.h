#ifndef FANCY_SEARCH_H
#define FANCY_SEARCH_H
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

#include "deliberate.h"
#include "delib-debug.h"

#include <QDialog>
#include "ui_fancysearch.h"
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include "utility-types.h"

class X { public: int x; };

namespace nota {

class FancySearch : public QDialog, public Ui_FancySearchDialog {

Q_OBJECT

public:

FancySearch (QWidget *parent);

void Setup (QSqlDatabase *db);

void Search ();

NoteIdSetType & ResultSet () { return noteSet; }
bool            DidSearch () { return didSearch; }

public slots:

  void DoSearch ();
  void AddAllTags ();
  void AddAllBooks ();
  
  void PickedTag (QListWidgetItem *item);
  void PickedBook (QListWidgetItem *item);
  
private:

  void SetRadios ();
  void RemoveItem (QListWidget *lit, QListWidgetItem *item);
  QStringList BookCond ();
  QStringList TagCond ();
  
  void MakeSet (NoteIdSetType &set, QStringList commands);

  QSqlDatabase   *pDB;

  
  bool          tagsAll;
  bool          tagsExcept;
  bool          tagsAny;
  bool          booksAll;
  bool          booksExcept;
  bool          booksAny;
  
  bool          didSearch;
  
  NoteIdSetType  noteSet;


};



} // namespace

#endif

