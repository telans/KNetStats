/***************************************************************************
*   Copyright (C) 2004-2005 by Hugo Parente Lima                               *
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

#include <arpa/inet.h>
#include <linux/netdevice.h>


#include "configure.h"
class QTimer;
class QMouseEvent;
class QPainter;
class QPaintEvent;
class Statistics;
class KNetStats;


/**
*	Main class
*/
class KNetStatsView : public KSystemTray
{
	Q_OBJECT

	enum {
		HISTORY_SIZE = 50,	// Tamanho do historico.
		SPEED_BUFFER_SIZE = 10	// Tamanho do buffer usado para calcular a velocidade
	};

public:
	KNetStatsView(KNetStats* parent, const QString& interface);
	~KNetStatsView();

	void updateViewOptions();
	// read a value from /sys/class/net/interface/name
	unsigned long readInterfaceNumValue(const char* name);
	// read a value from /sys/class/net/interface/name
	QString readInterfaceStringValue(const char* name, int maxlength);
	QString getIp();
	QString getNetmask();

	const double* speedHistoryRx() const { return mSpeedHistoryRx; }
	const double* speedHistoryTx() const { return mSpeedHistoryTx; }
	const int historyBufferSize() const { return HISTORY_SIZE; }
	const int* historyPointer() const { return &mSpeedHistoryPtr; }
	const double* maxSpeed() const { return &mMaxSpeed; }

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

	int mFdSock;					// Kernel-knetstats socket
	struct ifreq mDevInfo;			// info struct about our interface


	QString mSysDevPath;			// Path to the device.
	bool mCarrier;					// Interface carrier is on?
	bool mConnected;				// Interface exists?

	KPopupMenu* mContextMenu;		// Global ContextMenu
	Statistics* mStatistics;		// Statistics window
	QString mInterface;				// Current interface
	ViewOptions mOptions;			// View options

	// Icons
	QPixmap mIconError, mIconNone, mIconTx, mIconRx, mIconBoth;
	QPixmap* mCurrentIcon;			// Current state
	QTimer* mTimer;					// Timer

	//	Rx e Tx to bytes and packets
	unsigned long mBRx, mBTx, mPRx, mPTx;
	// Statistics
	unsigned long mTotalBytesRx, mTotalBytesTx, mTotalPktRx, mTotalPktTx;
	// Speed buffers
	double mSpeedBufferRx[SPEED_BUFFER_SIZE], mSpeedBufferTx[SPEED_BUFFER_SIZE];
	double mSpeedBufferPRx[SPEED_BUFFER_SIZE], mSpeedBufferPTx[SPEED_BUFFER_SIZE];
	// pointer to current speed buffer position
	int mSpeedBufferPtr;

	bool mFirstUpdate;

	// History buffer TODO: Make it configurable!
	double mSpeedHistoryRx[HISTORY_SIZE];
	double mSpeedHistoryTx[HISTORY_SIZE];
	int mSpeedHistoryPtr;
	double mMaxSpeed;
	int mMaxSpeedAge;


	// setup the view.
	void setup();
	void drawText(QPainter& paint);
	void drawGraphic(QPainter& paint);
	// Reset speed and history buffers
	void resetBuffers();
	// calc tha max. speed stored in the history buffer
	inline void calcMaxSpeed();
	// calc the speed using a speed buffer
	inline double calcSpeed(const double* buffer) const;

	bool openFdSocket();

private slots:
	// Called by the timer to update statistics
	void updateStats();

};

void KNetStatsView::calcMaxSpeed() {
	double max = 0.0;
	int ptr = mSpeedHistoryPtr;
	for (int i = 0; i < HISTORY_SIZE; ++i) {
		if (mSpeedHistoryRx[i] > max) {
			max = mSpeedHistoryRx[i];
			ptr = i;
		}
		if (mSpeedHistoryTx[i] > max) {
			max = mSpeedHistoryTx[i];
			ptr = i;
		}
	}
	mMaxSpeed = max;
	mMaxSpeedAge = (mSpeedHistoryPtr > ptr) ? (mSpeedHistoryPtr - ptr) : (mSpeedHistoryPtr + HISTORY_SIZE - ptr);
}

double KNetStatsView::calcSpeed(const double* buffer) const
{
	double total = 0.0;
	for (int i = 0; i < SPEED_BUFFER_SIZE; ++i)
		total += buffer[i];
	return total/SPEED_BUFFER_SIZE;
}

const QString& KNetStatsView::interface() const
{
	return mInterface;
}

int KNetStatsView::updateInterval() const
{
	return mOptions.mUpdateInterval;
}

ViewMode KNetStatsView::viewMode() const
{
	return mOptions.mViewMode;
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
	return mSpeedHistoryRx[mSpeedHistoryPtr];
}

double KNetStatsView::byteSpeedTx() const
{
	return mSpeedHistoryTx[mSpeedHistoryPtr];
}

double KNetStatsView::pktSpeedRx() const
{
	return calcSpeed(mSpeedBufferPRx);
}

double KNetStatsView::pktSpeedTx() const
{
	return calcSpeed(mSpeedBufferPTx);
}

#endif
