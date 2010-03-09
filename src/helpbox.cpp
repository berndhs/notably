#include "helpbox.h"

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


namespace nota {

HelpBox::HelpBox (QWidget * parent)
:QDialog(parent)
{
  setupUi (this);
  Connect ();
  hide ();
}

void
HelpBox::setText (const QString &text)
{
  textBox->setPlainText (text);
}

void
HelpBox::Connect ()
{
  connect (okButton, SIGNAL (clicked()), this, SLOT (DoOk()));
  connect (helpButton, SIGNAL (clicked()), this, SLOT (DoHelp()));
  connect (onlineButton, SIGNAL (clicked()), this, SLOT (DoOnline()));
  connect (licenseButton, SIGNAL (clicked()),
            this, SLOT (DoLicense ()));
}

void
HelpBox::DoOk ()
{
  accept ();
}

void
HelpBox::DoHelp ()
{
  emit WantHelp ();
  accept ();
}

void
HelpBox::DoOnline ()
{
  emit WantOnline ();
  accept ();
}

void
HelpBox::DoLicense ()
{
  emit WantLicense ();
  accept ();
}

}
