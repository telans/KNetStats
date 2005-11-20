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
#include "statistics.h"
#include "chart.h"
#include "knetstatsview.h"

#include <klocale.h>
#include <kapplication.h>
#include <kactivelabel.h>
#include <qtimer.h>
#include <qlayout.h>



Statistics::Statistics( KNetStatsView* parent, const char *name )
: StatisticsBase( parent, name ), mInterface(parent->interface()), mParent(parent) {
	setCaption( i18n( "Details of %1" ).arg( mInterface ) );
	
	QBoxLayout* l = new QHBoxLayout( mChart );
	l->setAutoAdd( TRUE );
	Chart* chart = new Chart(mChart, parent->speedHistoryTx(), parent->speedHistoryRx(), parent->historyBufferSize(), parent->historyPointer(), parent->maxSpeed());
	mMAC->setText(mParent->readInterfaceStringValue("address", 18));
	mIP->setAlignment(Qt::AlignRight);
	mMAC->setAlignment(Qt::AlignRight);
	mNetmask->setAlignment(Qt::AlignRight);
	update();

	mTimer = new QTimer( this );
	connect( mTimer, SIGNAL( timeout() ), this, SLOT( update() ) );
	connect( mTimer, SIGNAL( timeout() ), chart, SLOT( update() ) );
}

void Statistics::update() {
	mMaxSpeed->setText(byteFormat( *mParent->maxSpeed(), 1, " B" )+"/s");
	mBRx->setText( byteFormat( mParent->totalBytesRx() ) );
	mBTx->setText( byteFormat( mParent->totalBytesTx() ) );
	mByteSpeedRx->setText( byteFormat( mParent->byteSpeedRx(), 1, " B" )+"/s" );
	mByteSpeedTx->setText( byteFormat( mParent->byteSpeedTx(), 1, " B" )+"/s" );

	mPRx->setText( QString::number( mParent->totalPktRx() ) );
	mPTx->setText( QString::number( mParent->totalPktTx() ) );
	mPktSpeedRx->setText( QString::number( mParent->pktSpeedRx(), 'f', 1 )+"pkts/s" );
	mPktSpeedTx->setText( QString::number( mParent->pktSpeedTx(), 'f', 1 )+"pkts/s" );
}

void Statistics::show() {
	// Update details...
	mMTU->setText(mParent->readInterfaceStringValue("mtu", 6));
	mIP->setText( mParent->getIp() );
	mNetmask->setText( mParent->getNetmask() );
	
	mTimer->start( mParent->updateInterval() );
	StatisticsBase::show();
}

void Statistics::accept() {
	mTimer->stop();
	StatisticsBase::accept();
}


#include "statistics.moc"
