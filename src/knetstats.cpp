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


#include <iostream>

KNetStats::KNetStats() : QWidget(0, "knetstats")
{
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
	QStringList views = cfg->readListEntry("Views");

	if (!views.size())	// no views... =/, mostra tela de configuração
	{
		this->configure();
	}
	else
	{
	std::cout << "#views: " << views.size() << std::endl;
		// inicia as views necessarias.
		for (QStringList::Iterator i = views.begin(); i != views.end(); ++i)
		{
			KNetStatsView* kview = new KNetStatsView(this, *i, getViewOpt(*i));
			mView[*i] = kview;
	std::cout << "monitorando " << *i << " - " << std::endl;
		}
	}

}

KNetStats::~KNetStats()
{
}

ViewOpts* KNetStats::getViewOpt( const QString& interface )
{
	KConfig* cfg = kapp->config();
	QFont defaultFont = font();
	KIconLoader* loader = kapp->iconLoader();

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
	view->mPathRx = cfg->readEntry("IconRx", "icon_rx.png");
	view->mIconRx = loader->loadIcon(view->mPathRx, KIcon::Panel, ICONSIZE);
	view->mPathTx = cfg->readEntry("IconTx", "icon_tx.png");
	view->mIconTx = loader->loadIcon(view->mPathTx, KIcon::Panel, ICONSIZE);
	view->mPathBoth = cfg->readEntry("IconBoth", "icon_both.png");
	view->mIconBoth = loader->loadIcon(view->mPathBoth, KIcon::Panel, ICONSIZE);
	view->mPathNone = cfg->readEntry("IconNone", "icon_none.png");
	view->mIconNone = loader->loadIcon(view->mPathNone, KIcon::Panel, ICONSIZE);
	view->mPathError = cfg->readEntry("IconError", "icon_error.png");
	view->mIconError = loader->loadIcon(view->mPathError, KIcon::Panel, ICONSIZE);

	return view;
}

QStringList KNetStats::searchInterfaces()
{
	FILE* fp = fopen("/proc/net/dev", "r");

	QStringList list;

	if (!fp)
		return list;
	char interface[8];
	char buffer[128];

	// Ignore header...
	fgets(buffer, sizeof(buffer), fp);
	fgets(buffer, sizeof(buffer), fp);
	while(fgets(buffer, sizeof(buffer), fp))
	{
		sscanf(buffer, " %[^ \t\r\n:]", interface);
		list.append(interface);
	}
	return list;
}

void KNetStats::configure()
{
	Configure dlg(this);
	int res = dlg.exec();
	if (res == QDialog::Accepted )
	{
std::cout << "Ok, verificando mudanças...\n";
		const ViewsMap& map = dlg.currentConfig();
		for (ViewsMap::ConstIterator i = map.begin(); i != map.end(); ++i)
		{
std::cout << i.key() << "?\n";
			if (i.data().mMonitoring)
			{
				// Verifica se ja esta sendo monitorada
				TrayIconMap::Iterator it = mView.find(i.key());
				if (it == mView.end())
				{
					ViewOpts* view = new ViewOpts(i.data());
					KNetStatsView* kview = new KNetStatsView(this, i.key(), view);
					mView[i.key()] = kview;
				}
				else
				{
					// TODO: Atualizar a configuração dos trayicon já ativos
				}
			}
			else
			{
				// Verificar se existe um trayicon, e apagar o mesmo!
			}
		}

	}
}

void KNetStats::about()
{
	KAboutApplication dlg(this);
	dlg.exec();
}
