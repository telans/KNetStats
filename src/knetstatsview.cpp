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
#include "knetstatsview.h"
#include "knetstats.h"

// KDE headers
#include <kapplication.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kaboutapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <kaction.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <khelpmenu.h>
#include <kpassivepopup.h>

// Qt headers
#include <qtimer.h>
#include <qfile.h>
#include <qtooltip.h>
#include <qcursor.h>
#include <qevent.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>

// C headers
#include <cstring>
#include <cstdio>

#include "configure.h"
#include "statistics.h"

extern const char* programName;

KNetStatsView::KNetStatsView(KNetStats* parent, const QString& interface, ViewOpts* view)
	: mSysDevPath("/sys/class/net/"+interface+"/"), KSystemTray(parent, 0), mInterface(interface),mView(view) {
	mFirstUpdate = true;
	mMaxSpeedAge = 0;
	mMaxSpeed = 0.0;
	mBRx = mBTx = mPRx = mPTx = 0;
	mConnected = mCarrier = true;
	mTotalBytesRx = mTotalBytesTx = mTotalPktRx = mTotalPktTx = 0;
	mSpeedBufferPtr = mSpeedHistoryPtr = 0;
	
	mStatistics = 0;
	resetBuffers();

	setTextFormat(Qt::PlainText);
	show();

	// Context menu
	mContextMenu = parent->contextMenu();

	// Timer
	mTimer = new QTimer(this, "timer");
	connect(mTimer, SIGNAL(timeout()), this, SLOT(updateStats(void)));

	QToolTip::add(this, i18n("Monitoring %1").arg(mInterface));
	setup();
	mStatistics = new Statistics(this);
}

void KNetStatsView::setup() {
	if (mView->mViewMode == Text)
		setFont(mView->mTxtFont);
	else if (mView->mViewMode == Icon) {
		// Load Icons
		KIconLoader* loader = kapp->iconLoader();
		mIconError = loader->loadIcon("theme"+QString::number(mView->mTheme)+"_error.png",
							KIcon::Panel, ICONSIZE);
		mIconNone = loader->loadIcon("theme"+QString::number(mView->mTheme)+"_none.png",
							KIcon::Panel, ICONSIZE);
		mIconTx = loader->loadIcon("theme"+QString::number(mView->mTheme)+"_tx.png",
						KIcon::Panel, ICONSIZE);
		mIconRx =loader->loadIcon("theme"+QString::number(mView->mTheme)+"_rx.png",
						KIcon::Panel, ICONSIZE);
		mIconBoth = loader->loadIcon("theme"+QString::number(mView->mTheme)+"_both.png",
							KIcon::Panel, ICONSIZE);
		mCurrentIcon = (mConnected && mCarrier) ? &mIconNone : &mIconError;
	}
	mTimer->start(mView->mUpdateInterval);
	updateStats();
	QWidget::update();
	mFirstUpdate = false;
}

KNetStatsView::~KNetStatsView() {
	delete mView;
}

void KNetStatsView::setViewOpts( ViewOpts* view ) {
	delete mView;
	mView = view;
	setup();
}

void KNetStatsView::updateStats()
{
	FILE* fp = fopen((mSysDevPath+"carrier").latin1(), "r");
	
	if (!fp && mConnected) { // interface caiu...
		mConnected = false;
		mCurrentIcon = &mIconError;
		
		resetBuffers();
		KPassivePopup::message(programName, i18n("%1 is inactive").arg(mInterface), kapp->miniIcon(), this);
	} else if (fp && !mConnected) {
		mConnected = true;
		KPassivePopup::message(programName, i18n("%1 is active").arg(mInterface), kapp->miniIcon(), this);
	}
	int carrierFlag;
	if (fp) {
		carrierFlag = fgetc(fp);
		fclose(fp);
	}
	
	if (!mConnected)
		return;
	
	if (carrierFlag == '0') { // carrier down
		if (mCarrier) {
			mCarrier = false;
			KPassivePopup::message(programName, i18n("%1 is disconnected").arg(mInterface), kapp->miniIcon(), this);
		}
		return;
	} else if (!mCarrier) { // carrier up
		mCarrier = true;
		KPassivePopup::message(programName, i18n("%1 is connected").arg(mInterface), kapp->miniIcon(), this);
	}
	
	unsigned int brx = readValue("rx_bytes");
	unsigned int btx = readValue("tx_bytes");
	unsigned int prx = readValue("rx_packets");
	unsigned int ptx = readValue("tx_packets");
	
	
	QPixmap* newIcon;
	if (++mSpeedBufferPtr >= SPEED_BUFFER_SIZE)
		mSpeedBufferPtr = 0;
	
	// Calcula as velocidades
	mSpeedBufferTx[mSpeedBufferPtr] = ((btx - mBTx)*(1000.0f/mView->mUpdateInterval));
	mSpeedBufferRx[mSpeedBufferPtr] = ((brx - mBRx)*(1000.0f/mView->mUpdateInterval));
	mSpeedBufferPTx[mSpeedBufferPtr] = ((ptx - mPTx)*(1000.0f/mView->mUpdateInterval));
	mSpeedBufferPRx[mSpeedBufferPtr] = ((prx - mPRx)*(1000.0f/mView->mUpdateInterval));
	
	if (++mSpeedHistoryPtr >= HISTORY_SIZE)
		mSpeedHistoryPtr = 0;
	mSpeedHistoryRx[mSpeedHistoryPtr] = calcSpeed(mSpeedBufferRx);
	mSpeedHistoryTx[mSpeedHistoryPtr] = calcSpeed(mSpeedBufferTx);
	
	if (!mFirstUpdate) { // a primeira velocidade sempre eh absurda, para evitar isso temos o mFirstUpdate
		mMaxSpeedAge--;
		
		if (mSpeedHistoryTx[mSpeedHistoryPtr] > mMaxSpeed) {
			mMaxSpeed = mSpeedHistoryTx[mSpeedHistoryPtr];
			mMaxSpeedAge = HISTORY_SIZE;
		}
		if (mSpeedHistoryRx[mSpeedHistoryPtr] > mMaxSpeed)  {
			mMaxSpeed = mSpeedHistoryRx[mSpeedHistoryPtr];
			mMaxSpeedAge = HISTORY_SIZE;
		}
		if (mMaxSpeedAge < 1)
			calcMaxSpeed();
	}
	
	if (mView->mViewMode == Icon) {
		if (brx == mBRx) {
			if (btx == mBTx )
				newIcon = &mIconNone;
			else
				newIcon = &mIconTx;
		} else {
			if (btx == mBTx )
				newIcon = &mIconRx;
			else
				newIcon = &mIconBoth;
		}

		if (newIcon != mCurrentIcon) {
			mCurrentIcon = newIcon;
			QWidget::update();
		}
	}else if (mView->mViewMode == Graphic || (btx != mBTx && brx != mBRx))
		QWidget::update();
	
	// Update stats
	mTotalBytesRx += brx - mBRx;
	mTotalBytesTx += btx - mBTx;
	mTotalPktRx += prx - mPRx;
	mTotalPktTx += ptx - mPTx;

	mBRx = brx;
	mBTx = btx;
	mPRx = prx;
	mPTx = ptx;

}

unsigned long KNetStatsView::readValue(const char* name) {
	// stdio functions appear to be more fast than QFile?
	FILE* fp = fopen((mSysDevPath+"statistics/"+name).latin1(), "r");
	long retval;
	fscanf(fp, "%lu", &retval);
	fclose(fp);
	return retval;
}

void KNetStatsView::resetBuffers() {
	memset(mSpeedHistoryRx, 0, sizeof(double)*HISTORY_SIZE);
	memset(mSpeedHistoryTx, 0, sizeof(double)*HISTORY_SIZE);
	memset(mSpeedBufferRx, 0, sizeof(double)*SPEED_BUFFER_SIZE);
	memset(mSpeedBufferTx, 0, sizeof(double)*SPEED_BUFFER_SIZE);
	memset(mSpeedBufferPRx, 0, sizeof(double)*SPEED_BUFFER_SIZE);
	memset(mSpeedBufferPTx, 0, sizeof(double)*SPEED_BUFFER_SIZE);
}

void KNetStatsView::paintEvent( QPaintEvent* ev )
{
	QPainter paint(this);
	switch(mView->mViewMode) {
		case Icon:
			paint.drawPixmap(0, 0, *mCurrentIcon);
			break;
		case Text:
			drawText(paint);
			break;
		case Graphic:
			drawGraphic(paint);
			break;
	}
}

void KNetStatsView::drawText(QPainter& paint) {
	paint.setFont( mView->mTxtFont );
	paint.setPen( mView->mTxtUplColor );
	paint.drawText( rect(), Qt::AlignTop, Statistics::byteFormat(byteSpeedTx(), 1, "", "KB", "MB"));
	paint.setPen( mView->mTxtDldColor );
	paint.drawText( rect(), Qt::AlignBottom, Statistics::byteFormat(byteSpeedRx(), 1, "", "KB", "MB"));
}

void KNetStatsView::drawGraphic(QPainter& paint) {
	//	paint.setBackgroundColor(Qt::black);
	QSize size = this->size();
	//	paint.fillRect(0, 0, size.width(), size.height(), Qt::SolidPattern);

	int step = size.width()/HISTORY_SIZE;
	if (step < 1)
		step = 1;
	const int maxHeight = size.height()-1;
	
	qDebug("MaxSpeed: %d, age: %d", int(mMaxSpeed), mMaxSpeedAge);
	int lastX;
	int lastRxY = maxHeight - int(maxHeight * (mSpeedHistoryRx[mSpeedHistoryPtr]/mMaxSpeed));
	int lastTxY = maxHeight - int(maxHeight * (mSpeedHistoryTx[mSpeedHistoryPtr]/mMaxSpeed));
	int x = lastX = size.width();
	int count = 0;
	for (int i = mSpeedHistoryPtr; count < HISTORY_SIZE; i--) {
		if (i < 0)
			i = HISTORY_SIZE-1;
		
		int rxY = maxHeight - int(maxHeight * (mSpeedHistoryRx[i]/mMaxSpeed));
		int txY = maxHeight - int(maxHeight * (mSpeedHistoryTx[i]/mMaxSpeed));
		paint.setPen(Qt::blue);
		paint.drawLine(lastX, lastRxY, x, rxY);
		paint.setPen(Qt::red);
		paint.drawLine(lastX, lastTxY, x, txY);
		qDebug("%d => %d", i, int(mSpeedHistoryRx[i]));
		lastX = x;
		lastRxY = rxY;
		lastTxY = txY;
		
		count++;
		x-=step;
	}
}

void KNetStatsView::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::RightButton )
		mContextMenu->exec(QCursor::pos());
	else if (ev->button() == Qt::LeftButton)
		statistics();
}


void KNetStatsView::statistics()
{
	if (mStatistics->isShown())
		mStatistics->accept();
	else
		mStatistics->show();
}

#include "knetstatsview.moc"
