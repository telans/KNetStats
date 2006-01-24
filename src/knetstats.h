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

#ifndef KNETSTATS_H
#define KNETSTATS_H

#include <qwidget.h>
#include <set>
#include <qstring.h>
#include "configure.h"

class KNetStatsView;
class KConfig;
class KPopupMenu;
class KActionCollection;


class KNetStats : public QWidget
{
	Q_OBJECT
public:
	/// Constructor
	KNetStats();

	KPopupMenu* const contextMenu() const { return mContextMenu; }
	/**
	*	\brief Search for network interfaces parsing /proc/net/dev
	*	\return A StringList with the network interface names
	*/
	static QStringList searchInterfaces();
	static void readInterfaceOptions(const QString& interface, ViewOptions* opts);

	/**
	*	Exists at least 1 view?
	*/
	bool canStart() const {
		return mCanStart;
	}


public slots:
	/// Display configure the dialog box
	bool configure();
	/// Configure dialog OK button
	void configOk();
	/// Configure dialog Apply button
	void configApply();
	/// Configure dialog Cancel button
	void configCancel();

	/// Diaplay the about box
	void about();

private:
	bool mCanStart;
	KPopupMenu* mContextMenu; // help menu usado por todos os tray icons
	Configure* mConfigure;
	KActionCollection* mActionCollection;
	typedef QMap<QString, KNetStatsView*> TrayIconMap;
	TrayIconMap mViews;

	void readOptions();
	void saveConfig(const OptionsMap& options);
	void clearOptions(OptionsMap* options);
};

#endif
