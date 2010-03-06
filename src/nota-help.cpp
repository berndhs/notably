#include "nota-help.h"
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

NotaHelp::NotaHelp (QWidget *parent)
:QDialog (parent)
{
  setupUi (this);
  ConnectButtons ();
  hide ();
}

NotaHelp::~NotaHelp ()
{}

void
NotaHelp::update ()
{
  box->update ();
  QWidget::update ();
}

void
NotaHelp::ConnectButtons ()
{
  connect (closeButton, SIGNAL (clicked()), this, SLOT (DoClose()));
  connect (backButton, SIGNAL (clicked()), this, SLOT (DoBack()));
  connect (forwardButton, SIGNAL (clicked()), this, SLOT (DoForward()));
}

void
NotaHelp::DoClose ()
{
  hide();
}

void
NotaHelp::DoBack ()
{
  box->back ();
}

void
NotaHelp::DoForward ()
{
  box->forward();
}

void
NotaHelp::ShowPage (QString urlString)
{
  QUrl url (urlString);
  box->load (url);
  show ();
  raise ();
  box->setFocus();
}


} // namespace

