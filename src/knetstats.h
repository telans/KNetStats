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

#ifndef _knetstats_h_
#define _knetstats_h_

#include <ksystemtray.h>
#include <qpixmap.h>
#include <qstringlist.h>

class QTimer;
class QMouseEvent;

/**
*	Main class
*/
class KNetStats : public KSystemTray
{
	Q_OBJECT
private:
	///	Icons
	QPixmap mIconBoth, mIconRx, mIconTx, mIconNone, mIconError;
	/// Current state
	QPixmap* mCurrentIcon;
	/// Timer
	QTimer* mTimer;
	///	Rx e Tx to bytes and packets
	unsigned int mBRx, mBTx, mPRx, mPTx;
	/// Statistics
	unsigned int mTotalBytesRx, mTotalBytesTx, mTotalPktRx, mTotalPktTx;
	/// Current interface
	QString mInterface;
	/// Current update interval
	int mUpdateInterval;

	/// is connected?
	bool mbConnected;

public:
	/// Default constructor
	KNetStats();

	///	The current monitored network interface
	inline const QString& interface() const;
	///	The current Update Interval in miliseconds
	inline int updateInterval() const;

	/// Total of bytes receiveds
	inline unsigned int totalBytesRx() const;
	/// Total of bytes transmitted
	inline unsigned int totalBytesTx() const;
	/// Total of packets receiveds
	inline unsigned int totalPktRx() const;
	/// Total of packets transmitted
	inline unsigned int totalPktTx() const;

	/**
	*	\brief Search for network interfaces parsing /proc/net/dev
	*	\return A StringList with the network interface names
	*/
	static QStringList searchInterfaces();
protected:
	void mousePressEvent( QMouseEvent* ev );

private slots:
	/// Called by the timer to update statistics
	void update();
	/// Display configure the dialog box
	void configure();
	/// Diaplay the about box
	void about();
	/// Display the statistics dialog box
	void statistics();

};

const QString& KNetStats::interface() const
{
	return mInterface;
}

int KNetStats::updateInterval() const
{
	return mUpdateInterval;
}

unsigned int KNetStats::totalBytesRx() const
{
	return mTotalBytesRx;
}

unsigned int KNetStats::totalBytesTx() const
{
	return mTotalBytesTx;
}

unsigned int KNetStats::totalPktRx() const
{
	return mTotalPktRx;
}

unsigned int KNetStats::totalPktTx() const
{
	return mTotalPktTx;
}

#endif
