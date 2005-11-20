/***************************************************************************
*   Copyright (C) 2005 by Hugo Parente Lima                               *
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

#ifndef CHART_H
#define CHART_H

#include <qwidget.h>

class QPaintEvent;

class Chart : public QWidget {
public:
	Chart(QWidget* parent, const double* uploadBuffer, const double* downloadBuffer, int bufferSize, const int* ptr, const double* maxspeed);
	QSize sizeHint() { return QSize(200, 100); }
protected:	
	void paintEvent( QPaintEvent* ev );

private:
	const double* mUplBuffer;
	const double* mDldBuffer;
	const int mBufferSize;
	const int* mPtr;
	const double* mMaxSpeed;
};

#endif
