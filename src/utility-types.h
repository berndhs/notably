#ifndef UTILITY_TYPES_H
#define UTILITY_TYPES_H

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

#include <QString>
#include <QPixmap>
#include <map>

namespace nota {

typedef std::map <QString, QPixmap*> TagPixMapType;

typedef QSet<qint64>   NoteIdSetType;

}

#endif
