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

#include "configure.h"
#include "knetstats.h"

// Qt includes
#include <qstringlist.h>
#include <qlistbox.h>
#include <qcheckbox.h>
// Kde includes
#include <kapplication.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kfontrequester.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kmessagebox.h>


#include <iostream>


Configure::Configure(KNetStats* parent) : ConfigureBase(parent)
{
	KConfig* cfg = kapp->config();

	QStringList ifs = KNetStats::searchInterfaces();

	if (!ifs.size())
	{
	}
	else
	{
		// Load configuration
		QFont defaultFont = font();
		KIconLoader* loader = kapp->iconLoader();
		QPixmap iconV = loader->loadIcon("ok", KIcon::Small, 16);
		for(QStringList::Iterator i = ifs.begin(); i != ifs.end(); ++i)
		{
			KConfigGroupSaver groupSaver(cfg, *i);
			ViewOpts view;
			// general
			view.mUpdateInterval = cfg->readNumEntry("UpdateInterval", 300);
			view.mViewMode = (ViewMode)cfg->readNumEntry("ViewMode", 0);
			view.mMonitoring = cfg->readBoolEntry("Monitoring", true);
			// txt view
			view.mTxtFont = cfg->readFontEntry("TxtFont", &defaultFont);
			view.mTxtUplColor = cfg->readColorEntry("TxtUplColor", &Qt::red);
			view.mTxtDldColor = cfg->readColorEntry("TxtDldColor", &Qt::green);
			// IconView
			view.mPathRx = cfg->readEntry("IconRx", "icon_rx.png");
			view.mIconRx = loader->loadIcon(view.mPathRx, KIcon::Panel, ICONSIZE);
			view.mPathTx = cfg->readEntry("IconTx", "icon_tx.png");
			view.mIconTx = loader->loadIcon(view.mPathTx, KIcon::Panel, ICONSIZE);
			view.mPathBoth = cfg->readEntry("IconBoth", "icon_both.png");
			view.mIconBoth = loader->loadIcon(view.mPathBoth, KIcon::Panel, ICONSIZE);
			view.mPathNone = cfg->readEntry("IconNone", "icon_none.png");
			view.mIconNone = loader->loadIcon(view.mPathNone, KIcon::Panel, ICONSIZE);
			view.mPathError = cfg->readEntry("IconError", "icon_error.png");
			view.mIconError = loader->loadIcon(view.mPathError, KIcon::Panel, ICONSIZE);

			if (view.mMonitoring)
				mInterfaces->insertItem(iconV, *i);
			else
				mInterfaces->insertItem(*i);

			mConfig[*i] = view;
		}

		mInterfaces->setCurrentItem(0);
		changeInterface(mInterfaces->selectedItem());
	}

	connect(mInterfaces, SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(changeInterface(QListBoxItem*)));
	connect(mIconList, SIGNAL(selected(int)), this, SLOT(changeIcon(int)));

	connect(mMonitoring, SIGNAL(toggled(bool)), this, SLOT(monitoringToggled(bool)));
}

void Configure::changeInterface(QListBoxItem* item)
{
	QString interface = item->text();
	std::cout << "changeInterface(" << interface << ")\n";
	if (!mCurrentItem.isEmpty())
	{
		// Salvas as modificações passadas
		ViewOpts& oldview = mConfig[mCurrentItem];
		oldview.mMonitoring = mMonitoring->isChecked();
		oldview.mUpdateInterval = mUpdateInterval->value();
	}

	// Carrega as da nova interface
	ViewOpts& view = mConfig[interface];
	// general
	mMonitoring->setChecked(view.mMonitoring);
	mUpdateInterval->setValue(view.mUpdateInterval);
	mViewMode->setCurrentItem(view.mViewMode);
	// txt options
	mTxtUplColor->setColor(view.mTxtUplColor);
	mTxtDldColor->setColor(view.mTxtDldColor);
	mTxtFont->setFont( view.mTxtFont );
	// Icon options
	mIconList->changeItem( view.mIconTx, i18n("Upload"), 0);
	mIconList->changeItem( view.mIconRx, i18n("Download"), 1);
	mIconList->changeItem( view.mIconBoth, i18n("Both"), 2);
	mIconList->changeItem( view.mIconNone, i18n("None"), 3);
	mIconList->changeItem( view.mIconError, i18n("Error"), 4);

	mCurrentItem = interface;
}

void Configure::accept()
{
	// Salva a configuração
	QStringList views;
	bool ok = false;
	for(ViewsMap::Iterator i = mConfig.begin(); i != mConfig.end(); ++i)
		if (i.data().mMonitoring)
		{
			ok = true;
			break;
		}

	if (!ok)
	{
		KMessageBox::error(this, i18n("You need select at least one interface to monitor."));
		return;
	}

	KConfig* cfg = kapp->config();
	for(ViewsMap::Iterator i = mConfig.begin(); i != mConfig.end(); ++i)
	{
		KConfigGroupSaver groupSaver(cfg, i.key());
		ViewOpts& view = i.data();
		// general
		cfg->writeEntry("UpdateInterval", view.mUpdateInterval);
		cfg->writeEntry("ViewMode", view.mViewMode);
		cfg->writeEntry("Monitoring", view.mMonitoring);
		// txt view
		cfg->writeEntry("TxtFont", view.mTxtFont);
		cfg->writeEntry("TxtUplColor", view.mTxtUplColor);
		cfg->writeEntry("TxtDldColor", view.mTxtDldColor);
		// IconView
		cfg->writeEntry("IconRx", view.mPathRx);
		cfg->writeEntry("IconTx", view.mPathTx);
		cfg->writeEntry("IconBoth", view.mPathBoth);
		cfg->writeEntry("IconNone", view.mPathNone);
		cfg->writeEntry("IconError", view.mPathError);

		views.append(i.key());
	}
	cfg->writeEntry("Views", views);
	done(QDialog::Accepted);
}

void Configure::changeIcon(int id)
{
	// TODO: isso.
	// essa merda ja tava toda feita so q apaguei sem querer o arquivo...
}

void Configure::monitoringToggled( bool on )
{
	if (on)
		mInterfaces->changeItem(kapp->iconLoader()->loadIcon("ok", KIcon::Small, 16),
							mInterfaces->currentText(), mInterfaces->currentItem());
	else
		mInterfaces->changeItem(mInterfaces->currentText(), mInterfaces->currentItem());
}
