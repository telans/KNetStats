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

#include "knetstatsview.h"
#include <klocale.h>
#include <qtimer.h>
#include <kapplication.h>

Statistics::Statistics( KNetStatsView* parent, const char *name )
		: StatisticsBase( parent, name )
{
	setCaption( i18n( "Statistics for %1" ).arg( parent->interface() ) );
	update();

	mTimer = new QTimer( this );
	connect( mTimer, SIGNAL( timeout() ), this, SLOT( update() ) );
}

void Statistics::update()
{
	KNetStatsView* parent = static_cast<KNetStatsView*>( this->parent() );
	mBRx->setText( byteFormat( parent->totalBytesRx() ) );
	mBTx->setText( byteFormat( parent->totalBytesTx() ) );
	mByteSpeedRx->setText( byteFormat( parent->byteSpeedRx(), 1, " B" )+"/s" );
	mByteSpeedTx->setText( byteFormat( parent->byteSpeedTx(), 1, " B" )+"/s" );

	mPRx->setText( QString::number( parent->totalPktRx() ) );
	mPTx->setText( QString::number( parent->totalPktTx() ) );
	mPktSpeedRx->setText( QString::number( parent->pktSpeedRx(), 'f', 1 )+"pkts/s" );
	mPktSpeedTx->setText( QString::number( parent->pktSpeedTx(), 'f', 1 )+"pkts/s" );
}

void Statistics::show()
{

	mTimer->start( static_cast<KNetStatsView*>(parent())->updateInterval() );
	StatisticsBase::show();
}

void Statistics::accept()
{
	mTimer->stop();
	StatisticsBase::accept();
}
