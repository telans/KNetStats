/***************************************************************************
*   Copyright (C) 2004 by Hugo Parente Lima                               *
*   hugo_pl@users.sourceforge.net                                         *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "configure.h"
#include "knetstats.h"

#include <knuminput.h>
#include <kcombobox.h>
#include <qstringlist.h>

Configure::Configure( KNetStats *parent, const char *name )
		: ConfigureBase( parent, name )
{
	mInterface->insertStringList( KNetStats::searchInterfaces() );
	mInterface->setCurrentItem( parent->interface() );
	mUpdateInterval->setValue( parent->updateInterval() );
}

unsigned int Configure::updateInterval() const
{
	return mUpdateInterval->value();
}

const QString Configure::interface() const
{
	return mInterface->currentText();
}
