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
			KNetStatsView* kview = new KNetStatsView(this, *i, getViewOpt(*i));
			mView[*i] = kview;
		}
	}
}

// move it to knetstats view constructor?
ViewOpts* KNetStats::getViewOpt( const QString& interface ) {
	KConfig* cfg = kapp->config();
	QFont defaultFont = font();

	KConfigGroupSaver groupSaver(cfg, interface);
	ViewOpts* view = new ViewOpts;
	// general
	view->mUpdateInterval = cfg->readNumEntry("UpdateInterval", 300);
	view->mViewMode = (ViewMode)cfg->readNumEntry("ViewMode", 0);
	view->mMonitoring = cfg->readBoolEntry("Monitoring", true);
	// txt view
	view->mTxtFont = cfg->readFontEntry("TxtFont", &defaultFont);
	view->mTxtUplColor = cfg->readColorEntry("TxtUplColor", &Qt::red);
	view->mTxtDldColor = cfg->readColorEntry("TxtDldColor", &Qt::green);
	// IconView
	view->mTheme = cfg->readNumEntry("Theme", 0);

	return view;
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
	if (mConfigure->saveConfig()) {
		applyConfig( mConfigure->currentConfig() );
		delete mConfigure;
		mConfigure = 0;
	}
}

void KNetStats::configApply()
{
	if (mConfigure->saveConfig())
		applyConfig( mConfigure->currentConfig() );
}

void KNetStats::configCancel()
{
	delete mConfigure;
	mConfigure = 0;

	if (!mView.size())
		kapp->quit();
}

void KNetStats::applyConfig(const ViewsMap& map)
{
	for (ViewsMap::ConstIterator i = map.begin(); i != map.end(); ++i) {
		if (i.data().mMonitoring) {
			// Verifica se ja esta sendo monitorada
			TrayIconMap::Iterator it = mView.find(i.key());
			if (it == mView.end()) {
				ViewOpts* view = new ViewOpts(i.data()); // coping data.. hmm... ugly!
				KNetStatsView* kview = new KNetStatsView(this, i.key(), view);
				mView[i.key()] = kview;
			} else
				it.data()->setViewOpts( new ViewOpts(i.data()) );
		} else {
			// Verificar se existe um trayicon, e apagar o mesmo!
			TrayIconMap::Iterator it = mView.find(i.key());
			if (it != mView.end()) {
				delete it.data();
				mView.erase(it);
			}
		}
	}
}

void KNetStats::about()
{
	KAboutApplication dlg(this);
	dlg.exec();
}

#include "knetstats.moc"
