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
#include <kapp.h>
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
#include <iostream>

#include "configure.h"
#include "statistics.h"

extern const char* programName;

KNetStatsView::KNetStatsView(KNetStats* parent, const QString& interface, ViewOpts* view)
	: KSystemTray(parent, 0), mStatistics(0), mInterface(interface),mView(view), mBRx(0), mBTx(0), mPRx(0), mPTx(0), mTotalBytesRx(0), mTotalBytesTx(0), mTotalPktRx(0), mTotalPktTx(0), mSpeedRx(0), mSpeedTx(0), mbConnected(true)
{

	std::cout << "Fui criado!! vou monitorar " << interface << "\n";
	setTextFormat(Qt::PlainText);

	// Context menu
	mContextMenu = parent->contextMenu();

	// Timer
	mTimer = new QTimer(this, "timer");
	connect(mTimer, SIGNAL(timeout()), this, SLOT(update(void)));

	// Load config
	QToolTip::add(this, i18n("Monitoring %1").arg(mInterface));

	setup();

	mStatistics = new Statistics(this);
	show();
}

void KNetStatsView::setup()
{
	if (mView->mViewMode == Text)
	{
		setFont(mView->mTxtFont);
		mSpeedTx = mSpeedRx = 0;
		mTimer->start(1000);
	}
	else if (mView->mViewMode == Icon)
	{
		mCurrentIcon = &mIconNone;
		mTimer->start(mView->mUpdateInterval);
		setPixmap( *mCurrentIcon );
	}
	update();
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

void KNetStatsView::update()
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
			mSpeedTx = ((btx - mBTx)*(1000.0f/mView->mUpdateInterval));
			mSpeedRx = ((brx - mBRx)*(1000.0f/mView->mUpdateInterval));
			mSpeedPTx = ((ptx - mPTx)*(1000.0f/mView->mUpdateInterval));
			mSpeedPRx = ((prx - mPRx)*(1000.0f/mView->mUpdateInterval));

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
					setPixmap( *mCurrentIcon );
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
			break;
		}
	}

	fclose(fp);

	if (!linkok && mbConnected)
	{
		mbConnected = false;
		mCurrentIcon = &mIconError;
		setPixmap(*mCurrentIcon);
		KPassivePopup::message(programName, i18n("%1 is inactive").arg(mInterface), kapp->miniIcon(), this);
		mSpeedRx = mSpeedTx = mSpeedPRx = mSpeedPTx = 0;
	}

}

void KNetStatsView::paintEvent( QPaintEvent* ev )
{
	QPainter paint(this);
	switch(mView->mViewMode)
	{
		case Icon:
			paint.drawPixmap(0, 0, *pixmap());
			break;
		case Text:
		{
			paint.setFont( mView->mTxtFont );
			paint.setPen( mView->mTxtUplColor );
			paint.drawText( rect(), Qt::AlignTop, Statistics::byteFormat(mSpeedTx, 1, "", "KB", "MB"));
			paint.setPen( mView->mTxtDldColor );
			paint.drawText( rect(), Qt::AlignBottom, Statistics::byteFormat(mSpeedRx, 1, "", "KB", "MB"));
			break;
		}
		case Bars:
std::cout << "Bars!\n";
			break;
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
