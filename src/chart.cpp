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

#include "chart.h"
#include <qpainter.h>
#include <qbrush.h>

Chart::Chart(QWidget* parent, const double* uploadBuffer, const double* downloadBuffer, int bufferSize, const int* ptr, const double* maxspeed) : QWidget(parent), mUplBuffer(uploadBuffer), mDldBuffer(downloadBuffer), mBufferSize(bufferSize), mPtr(ptr), mMaxSpeed(maxspeed) {
	setWFlags(Qt::WNoAutoErase);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Chart::paintEvent(QPaintEvent*) {
	QPainter paint(this);
	paint.setBackgroundColor(Qt::black);
	paint.setBackgroundMode(Qt::OpaqueMode);
	QBrush brush(QColor(0x33, 0x33, 0x33), CrossPattern);
	paint.fillRect(0, 0, width(), height(), brush);
	
	const double step = width()/double(mBufferSize);
	const int HEIGHT = height() - 1;
	
	int x;
	int lastX = x = width();
	int lastRxY = HEIGHT - int(HEIGHT * (mDldBuffer[*mPtr]/(*mMaxSpeed)));
	int lastTxY = HEIGHT - int(HEIGHT * (mUplBuffer[*mPtr]/(*mMaxSpeed)));
	
	int count = 0;
	for (int i = *mPtr; count < mBufferSize; i--) {
		if (i < 0)
			i = mBufferSize-1;
		int rxY = HEIGHT - int(HEIGHT * (mDldBuffer[i]/(*mMaxSpeed)));
		int txY = HEIGHT - int(HEIGHT * (mUplBuffer[i]/(*mMaxSpeed)));
		paint.setPen(Qt::green);
		paint.drawLine(lastX, lastRxY, x, rxY);
		paint.setPen(Qt::red);
		paint.drawLine(lastX, lastTxY, x, txY);
		//qDebug("%d => %d", i, int(mSpeedHistoryRx[i]));
		lastX = x;
		lastRxY = rxY;
		lastTxY = txY;
		
		count++;
		x = width() - int(step*(count+1));
		
	}
}

