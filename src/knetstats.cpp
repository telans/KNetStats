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

#include "knetstats.h"
#include "knetstatsview.h"
#include "configure.h"

// Qt includes
#include <qstringlist.h>
#include <qdir.h>
// KDE includes
#include <kconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <khelpmenu.h>
#include <kaboutapplication.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kpushbutton.h>
// StdC++ includes
#include <algorithm>

KNetStats::KNetStats() : QWidget(0, "knetstats"), mAllOk(true), mConfigure(0) {
	setIcon(kapp->icon());

	// Cria o contextMenu
	mActionCollection = new KActionCollection(this);
	mContextMenu = new KPopupMenu(this);
	mContextMenu->insertTitle( kapp->miniIcon(), kapp->caption() );
	KAction* configure = KStdAction::preferences(this, SLOT(configure()), mActionCollection, "configure");
	configure->plug(mContextMenu);
//	mContextMenu->insertItem( i18n("&Statistics"), this, SLOT(statistics()) );
	mContextMenu->insertSeparator();
	KHelpMenu* helpmenu = new KHelpMenu(this, QString::null, false);
	mContextMenu->insertItem( i18n("&Help"), helpmenu->menu() );

	mContextMenu->insertSeparator();
	KAction* quitAction = KStdAction::quit(kapp, SLOT(quit()), mActionCollection);
	quitAction->plug(mContextMenu);

	connect(helpmenu, SIGNAL(showAboutApplication()), this, SLOT(about()));

	// lê arquivo de configuração
	KConfig* cfg = kapp->config();
	QStringList views = cfg->readListEntry("CurrentViews");

	if (!views.size()) {	// no views... =/, mostra tela de configuração
		if (!this->configure())
			mAllOk = false;
	} else {
		// inicia as views necessarias.
		for (QStringList::Iterator i = views.begin(); i != views.end(); ++i) {
			ViewOptions* options = new ViewOptions;
			readInterfaceOptions(*i, options);
			KNetStatsView* kview = new KNetStatsView(this, *i, options);
			mViews[*i] = kview;
		}
	}
}


void KNetStats::readInterfaceOptions(const QString& interface, ViewOptions* opts) {
	
	KConfig* cfg = kapp->config();
	KConfigGroupSaver groupSaver(cfg, interface);
	QFont defaultFont = font();
	
	// general
	opts->mUpdateInterval = cfg->readNumEntry("UpdateInterval", 300);
	opts->mViewMode = (ViewMode)cfg->readNumEntry("ViewMode", 0);
	opts->mMonitoring = cfg->readBoolEntry("Monitoring", true);
	// txt view
	opts->mTxtFont = cfg->readFontEntry("TxtFont", &defaultFont);
	opts->mTxtUplColor = cfg->readColorEntry("TxtUplColor", &Qt::red);
	opts->mTxtDldColor = cfg->readColorEntry("TxtDldColor", &Qt::green);
	// IconView
	int defaultTheme = 0;
	if (interface.startsWith("wlan"))
		defaultTheme = 3;
	opts->mTheme = cfg->readNumEntry("Theme", defaultTheme);
	// Graphic
	opts->mChartUplColor = cfg->readColorEntry("ChartUplColor", &Qt::red);
	opts->mChartDldColor = cfg->readColorEntry("ChartDldColor", &Qt::blue);
	opts->mChartBgColor = cfg->readColorEntry("ChartBgColor", &Qt::white);
	opts->mChartTransparentBackground = cfg->readBoolEntry("ChartUseTransparentBackground", true);
}


QStringList KNetStats::searchInterfaces() {
	QDir dir("/sys/class/net");
	QStringList list = dir.entryList(QDir::Dirs);
	list.pop_front(); // removes "." and ".." entriess
	list.pop_front();
	return list;
}

bool KNetStats::configure() {
	if (mConfigure)
		mConfigure->show();
	else {
		// Procura interfaces de rede
		QStringList ifs = KNetStats::searchInterfaces();
		ifs += kapp->config()->readListEntry("AllViews");
		ifs.sort();
		ifs.erase( std::unique(ifs.begin(), ifs.end()), ifs.end() );
		kapp->config()->writeEntry("AllViews", ifs);

		if (!ifs.size()) {
			KMessageBox::error(this, i18n("You don't have any network interface.\nKNetStats will quit now."));
			return false;
		}
		
		mConfigure = new Configure(this, ifs);
		connect(mConfigure->mOk, SIGNAL(clicked()), this, SLOT(configOk()));
		connect(mConfigure->mApply, SIGNAL(clicked()), this, SLOT(configApply()));
		connect(mConfigure->mCancel, SIGNAL(clicked()), this, SLOT(configCancel()));
		mConfigure->show();
	}
	return true;
}

void KNetStats::configOk()
{
	if (mConfigure->canSaveConfig()) {
		saveConfig( mConfigure->options() );
		delete mConfigure;
		mConfigure = 0;
	}
}

void KNetStats::configApply()
{
	if (mConfigure->canSaveConfig())
		saveConfig( mConfigure->options() );
}

void KNetStats::configCancel()
{
	delete mConfigure;
	mConfigure = 0;
	
	if (!mViews.size())
		kapp->quit();
}

void KNetStats::saveConfig(const OptionsMap& options)
{
	KConfig* cfg = kapp->config();
	
	for(OptionsMap::ConstIterator i = options.begin(); i != options.end(); ++i) {
		KConfigGroupSaver groupSaver(cfg, i.key());
		const ViewOptions& opt = i.data();
		// general
		cfg->writeEntry("UpdateInterval", opt.mUpdateInterval);
		cfg->writeEntry("ViewMode", opt.mViewMode);
		cfg->writeEntry("Monitoring", opt.mMonitoring);
		// txt view
		cfg->writeEntry("TxtFont", opt.mTxtFont);
		cfg->writeEntry("TxtUplColor", opt.mTxtUplColor);
		cfg->writeEntry("TxtDldColor", opt.mTxtDldColor);
		// IconView
		cfg->writeEntry("Theme", opt.mTheme);
		// Graphic view
		cfg->writeEntry("ChartUplColor", opt.mChartUplColor);
		cfg->writeEntry("ChartDldColor", opt.mChartDldColor);
		cfg->writeEntry("ChartBgColor", opt.mChartBgColor);
		cfg->writeEntry("ChartUseTransparentBackground", opt.mChartTransparentBackground);
	
		TrayIconMap::Iterator trayIcon = mViews.find(i.key());
		if (opt.mMonitoring) {	// Verifica se ja esta sendo monitorada
			if (trayIcon == mViews.end()) { // nova interface!
				KNetStatsView* kview = new KNetStatsView(this, i.key(), new ViewOptions(opt));
				mViews[i.key()] = kview;
			} else
				trayIcon.data()->setViewOptions( new ViewOptions(opt) );
		} else
			// Verificar se existe um trayicon, e apagar o mesmo!
			if (trayIcon != mViews.end()) {
				delete trayIcon.data();
				mViews.erase(trayIcon);
			}
	}
	cfg->writeEntry("CurrentViews", mViews.keys());
}

void KNetStats::about()
{
	KAboutApplication dlg(this);
	dlg.exec();
}

#include "knetstats.moc"
