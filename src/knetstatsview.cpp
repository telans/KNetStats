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
	: KSystemTray(parent, 0), mInterface(interface),mView(view)
{
	mStatistics = 0;
	mBRx = mBTx = mPRx = mPTx = 0;
	mbConnected = true;
	mTotalBytesRx = mTotalBytesTx = mTotalPktRx = mTotalPktTx = 0;
	mPtr = 0;
	memset(mSpeedRx, 0, sizeof(double)*HISTORY_SIZE);
	memset(mSpeedTx, 0, sizeof(double)*HISTORY_SIZE);
	memset(mSpeedPRx, 0, sizeof(double)*HISTORY_SIZE);
	memset(mSpeedPTx, 0, sizeof(double)*HISTORY_SIZE);

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

void KNetStatsView::setup()
{
	if (mView->mViewMode == Text)
		setFont(mView->mTxtFont);
	else if (mView->mViewMode == Icon)
	{
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
		mCurrentIcon = mbConnected ? &mIconNone : &mIconError;
	}
	mTimer->start(mView->mUpdateInterval);
	updateStats();
	QWidget::update();
}

KNetStatsView::~KNetStatsView()
{
	delete mView;
}

void KNetStatsView::setViewOpts( ViewOpts* view )
{
	delete mView;
	mView = view;
	setup();
}

void KNetStatsView::updateStats()
{
	// Read and parse /proc/net/dev
	FILE* fp = fopen("/proc/net/dev", "r");
	if (!fp)
	{
		mTimer->stop();
		KMessageBox::error(this, i18n("Error opening /proc/net/dev!"));
		return;
	}

	unsigned int brx, btx, prx, ptx;
	char interface[8];
	char buffer[128];

	// skip headers (why not a fseek() ?)
	fgets(buffer, sizeof(buffer), fp);
	fgets(buffer, sizeof(buffer), fp);

	bool linkok = false;
	while(fgets(buffer, sizeof(buffer), fp))
	{
		// Read statistics
		sscanf(buffer, " %[^ \t\r\n:]%*c%u%u%*u%*u%*u%*u%*u%*u%u%u%*u%*u%*u%*u%*u%*u", interface, &brx, &prx, &btx, &ptx);

		if (interface == mInterface)
		{
			QPixmap* newIcon;

			// Calcula as velocidades
			mSpeedTx[mPtr] = ((btx - mBTx)*(1000.0f/mView->mUpdateInterval));
			mSpeedRx[mPtr] = ((brx - mBRx)*(1000.0f/mView->mUpdateInterval));
			mSpeedPTx[mPtr] = ((ptx - mPTx)*(1000.0f/mView->mUpdateInterval));
			mSpeedPRx[mPtr] = ((prx - mPRx)*(1000.0f/mView->mUpdateInterval));

			if (mView->mViewMode == Text)
				QWidget::update();
			else if (mView->mViewMode == Icon)
			{
				if (brx == mBRx)
				{
					if (btx == mBTx )
						newIcon = &mIconNone;
					else
						newIcon = &mIconTx;
				}
				else
				{
					if (btx == mBTx )
						newIcon = &mIconRx;
					else
						newIcon = &mIconBoth;
				}

				if (newIcon != mCurrentIcon)
				{
					mCurrentIcon = newIcon;
					QWidget::update();
				}
			}

			// Update stats
			mTotalBytesRx += brx - mBRx;
			mTotalBytesTx += btx - mBTx;
			mTotalPktRx += prx - mPRx;
			mTotalPktTx += ptx - mPTx;

			mBRx = brx;
			mBTx = btx;
			mPRx = prx;
			mPTx = ptx;

			if (!mbConnected)
			{
				mbConnected = true;
				KPassivePopup::message(programName, i18n("%1 is active").arg(mInterface), kapp->miniIcon(), this);
			}
			linkok = true;
			if (++mPtr >= HISTORY_SIZE)
				mPtr = 0;
			break;
		}
	}

	fclose(fp);

	if (!linkok && mbConnected)
	{
		mbConnected = false;
		mCurrentIcon = &mIconError;
		memset(mSpeedRx, 0, sizeof(double)*HISTORY_SIZE);
		memset(mSpeedTx, 0, sizeof(double)*HISTORY_SIZE);
		memset(mSpeedPRx, 0, sizeof(double)*HISTORY_SIZE);
		memset(mSpeedPTx, 0, sizeof(double)*HISTORY_SIZE);
		QWidget::update();
		KPassivePopup::message(programName, i18n("%1 is inactive").arg(mInterface), kapp->miniIcon(), this);
	}
}

void KNetStatsView::paintEvent( QPaintEvent* ev )
{
	QPainter paint(this);
	switch(mView->mViewMode)
	{
		case Icon:
			paint.drawPixmap(0, 0, *mCurrentIcon);
			break;
		case Text:
		{
			paint.setFont( mView->mTxtFont );
			paint.setPen( mView->mTxtUplColor );
			paint.drawText( rect(), Qt::AlignTop, Statistics::byteFormat(byteSpeedTx(), 1, "", "KB", "MB"));
			paint.setPen( mView->mTxtDldColor );
			paint.drawText( rect(), Qt::AlignBottom, Statistics::byteFormat(byteSpeedRx(), 1, "", "KB", "MB"));
			break;
		}
/*		case Bars:
			break;*/
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
