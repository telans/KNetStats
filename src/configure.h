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

#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "configurebase.h"
#include <qstring.h>
#include <qmap.h>
#include <qfont.h>
#include <qcolor.h>
#include <qpixmap.h>

static const int ICONSIZE = 22;

class KNetStats;
class QListBoxItem;

enum ViewMode {
	Icon, Text, Bars
};

struct ViewOpts
{
	// general
	int mUpdateInterval;
	ViewMode mViewMode;
	bool mMonitoring;
	// txt view
	QFont mTxtFont;
	QColor mTxtUplColor;
	QColor mTxtDldColor;
	// icon view
	QString mPathRx;
	QString mPathTx;
	QString mPathBoth;
	QString mPathNone;
	QString mPathError;
	QPixmap mIconRx;
	QPixmap mIconTx;
	QPixmap mIconBoth;
	QPixmap mIconNone;
	QPixmap mIconError;
};

typedef QMap<QString, ViewOpts> ViewsMap;

class Configure : public ConfigureBase
{
	Q_OBJECT
public:
	Configure(KNetStats* parent);

	const ViewsMap& currentConfig()
	{
		return mConfig;
	}
private:
	QString mCurrentItem;
	ViewsMap mConfig;
public slots:
	void accept();
protected slots:
	void changeInterface(QListBoxItem* item);
	void changeIcon(int id);
	void monitoringToggled(bool on);

};

#endif
