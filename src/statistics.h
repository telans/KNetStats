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
#ifndef STATISTICS_H
#define STATISTICS_H

#include "statisticsbase.h"
#include <qstring.h>

class KNetStats;

class Statistics : public StatisticsBase
{
	Q_OBJECT
public:
	Statistics( KNetStats *parent = 0, const char *name = 0 );

	static QString byteFormat( unsigned int num );
private slots:
	void update();
};

#endif
