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

#ifndef KNETSTATSVIEW_H
#define KNETSTATSVIEW_H

#include <ksystemtray.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qcolor.h>

#include "configure.h"
class QTimer;
class QMouseEvent;
class QPaintEvent;
class Statistics;
class KNetStats;

/**
*	Main class
*/
class KNetStatsView : public KSystemTray
{
	Q_OBJECT
public:
	/// Default constructor
	KNetStatsView(KNetStats* parent, const QString& interface, ViewOpts* view);
	~KNetStatsView();

	///	The current monitored network interface
	inline const QString& interface() const;
	///	The current Update Interval in miliseconds
	inline int updateInterval() const;
	/// We are in textmode?
	inline ViewMode viewMode() const;

	/// Total of bytes receiveds
	inline unsigned int totalBytesRx() const;
	/// Total of bytes transmitted
	inline unsigned int totalBytesTx() const;
	/// Total of packets receiveds
	inline unsigned int totalPktRx() const;
	/// Total of packets transmitted
	inline unsigned int totalPktTx() const;
	/// RX Speed in bytes per second
	inline double byteSpeedRx() const;
	/// TX Speed in bytes per second
	inline double byteSpeedTx() const;
	/// RX Speed in packets per second
	inline double pktSpeedRx() const;
	/// TX Speed in packets per second
	inline double pktSpeedTx() const;

protected:
	void mousePressEvent( QMouseEvent* ev );
	void paintEvent( QPaintEvent* ev );

private:
	/// Global ContextMenu
	KPopupMenu* mContextMenu;
	/// Statistics window
	Statistics* mStatistics;
	/// Current interface
	QString mInterface;
	/// View configuration
	ViewOpts* mView;

	/// Current state
	QPixmap* mCurrentIcon;
	/// Timer
	QTimer* mTimer;
	///	Rx e Tx to bytes and packets
	unsigned int mBRx, mBTx, mPRx, mPTx;
	/// Statistics
	unsigned int mTotalBytesRx, mTotalBytesTx, mTotalPktRx, mTotalPktTx;
	/// Speeds
	double mSpeedRx, mSpeedTx, mSpeedPRx, mSpeedPTx;
	/// is connected?
	bool mbConnected;

private slots:
	/// Called by the timer to update statistics
	void update();
	/// Display the statistics dialog box
	void statistics();

};

const QString& KNetStatsView::interface() const
{
	return mInterface;
}

int KNetStatsView::updateInterval() const
{
	return mView->mUpdateInterval;
}

ViewMode KNetStatsView::viewMode() const
{
	return mView->mViewMode;
}

unsigned int KNetStatsView::totalBytesRx() const
{
	return mTotalBytesRx;
}

unsigned int KNetStatsView::totalBytesTx() const
{
	return mTotalBytesTx;
}

unsigned int KNetStatsView::totalPktRx() const
{
	return mTotalPktRx;
}

unsigned int KNetStatsView::totalPktTx() const
{
	return mTotalPktTx;
}

double KNetStatsView::byteSpeedRx() const
{
	return mSpeedRx;
}

double KNetStatsView::byteSpeedTx() const
{
	return mSpeedTx;
}

double KNetStatsView::pktSpeedRx() const
{
	return mSpeedPRx;
}

double KNetStatsView::pktSpeedTx() const
{
	return mSpeedPTx;
}

#endif
