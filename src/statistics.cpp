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

#include "knetstats.h"
#include <klocale.h>
#include <qtimer.h>
#include <kapplication.h>

Statistics::Statistics( KNetStats* parent, const char *name )
		: StatisticsBase( parent, name )
{
	setCaption( i18n( "Statistics for %1" ).arg( parent->interface() ) );
	update();

	QTimer* timer = new QTimer( this );
	connect( timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	timer->start( parent->updateInterval() );
}

void Statistics::update()
{
	mBRx->setText( byteFormat( static_cast<KNetStats*>( parent() )->totalBytesRx() ) );
	mBTx->setText( byteFormat( static_cast<KNetStats*>( parent() )->totalBytesTx() ) );
	mPRx->setText( QString::number( static_cast<KNetStats*>( parent() )->totalPktRx() ) );
	mPTx->setText( QString::number( static_cast<KNetStats*>( parent() )->totalPktTx() ) );
}

QString Statistics::byteFormat( unsigned int num )
{
	if ( num > 1024 * 1024 ) 	// MB
		return QString::number( float( num ) / ( 1024 * 1024 ), 'f', 2 ) + " MB";
	else if ( num > 1024 ) 	// Kb
		return QString::number( float( num ) / 1024, 'f', 2 ) + " KB";
	else	// bytes
		return QString::number( num ) + " bytes";
}
