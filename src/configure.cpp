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
#include <qlabel.h>
#include <qwidgetstack.h>
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
#include <kpushbutton.h>


Configure::Configure(KNetStats* parent, const QStringList& ifs) : ConfigureBase(parent)
{
	KConfig* cfg = kapp->config();

	// Load configuration
	QFont defaultFont = font();
	KIconLoader* loader = kapp->iconLoader();

	QPixmap iconPCI = loader->loadIcon("icon_pci.png", KIcon::Small, 16);


	for(QStringList::ConstIterator i = ifs.begin(); i != ifs.end(); ++i)
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

		int defaultTheme = 0;
		if ((*i).startsWith("wlan"))
			defaultTheme = 3;
		mInterfaces->insertItem(iconPCI, *i);
		view.mTheme = cfg->readNumEntry("Theme", defaultTheme);

		mConfig[*i] = view;
	}

	mInterfaces->setCurrentItem(0);
	changeInterface(mInterfaces->selectedItem());

	connect(mInterfaces, SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(changeInterface(QListBoxItem*)));
	connect(mTheme, SIGNAL(activated(int)), this, SLOT(changeTheme(int)));
}

void Configure::changeInterface(QListBoxItem* item)
{
	QString interface = item->text();

	if (!mCurrentItem.isEmpty())
	{
		// Salvas as modificações passadas
		ViewOpts& oldview = mConfig[mCurrentItem];
		// general options
		oldview.mMonitoring = mMonitoring->isChecked();
		oldview.mUpdateInterval = mUpdateInterval->value();
		oldview.mViewMode = (ViewMode) mViewMode->currentItem();
		// txt view options
		oldview.mTxtUplColor = mTxtUplColor->color();
		oldview.mTxtDldColor = mTxtDldColor->color();
		oldview.mTxtFont = mTxtFont->font();
		// icon view
		oldview.mTheme = mTheme->currentItem();
	}

	if (interface == mCurrentItem)
		return;
	// Carrega as opt. da nova interface
	ViewOpts& view = mConfig[interface];
	// general
	mMonitoring->setChecked(view.mMonitoring);
	mUpdateInterval->setValue(view.mUpdateInterval);
	mViewMode->setCurrentItem(view.mViewMode);
	mWdgStack->raiseWidget(view.mViewMode);
	// txt options
	mTxtUplColor->setColor(view.mTxtUplColor);
	mTxtDldColor->setColor(view.mTxtDldColor);
	mTxtFont->setFont( view.mTxtFont );
	// icon options
	mTheme->setCurrentItem(view.mTheme);
	changeTheme(view.mTheme);

	mCurrentItem = interface;
}

bool Configure::saveConfig()
{
	// Atualiza o cache de opções
	changeInterface(mInterfaces->item( mInterfaces->currentItem() ));

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
		KMessageBox::error(this, i18n("You need to select at least one interface to monitor."));
		return false;
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
		cfg->writeEntry("Theme", view.mTheme);

		if (view.mMonitoring)
			views.append(i.key());
	}
	cfg->writeEntry("CurrentViews", views);
	return true;
}

void Configure::changeTheme(int theme)
{
	KIconLoader* loader = kapp->iconLoader();
	mIconError->setPixmap(loader->loadIcon("theme"+QString::number(theme)+"_error.png",
						  KIcon::Panel, ICONSIZE));
	mIconNone->setPixmap(loader->loadIcon("theme"+QString::number(theme)+"_none.png",
						 KIcon::Panel, ICONSIZE));
	mIconTx->setPixmap(loader->loadIcon("theme"+QString::number(theme)+"_tx.png",
					   KIcon::Panel, ICONSIZE));
	mIconRx->setPixmap(loader->loadIcon("theme"+QString::number(theme)+"_rx.png",
					   KIcon::Panel, ICONSIZE));
	mIconBoth->setPixmap(loader->loadIcon("theme"+QString::number(theme)+"_both.png",
						 KIcon::Panel, ICONSIZE));
}
#include "configure.moc"
