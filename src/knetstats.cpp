#include "knetstats.h"

// KDE headers
#include <kapp.h>
#include <kpopupmenu.h>
#include <kmessagebox.h>
#include <kaboutapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <kaction.h>
#include <kstdaccel.h>
#include <kstdaction.h>
#include <khelpmenu.h>
#include <kpassivepopup.h>

// Qt headers
#include <qtimer.h>
#include <qfile.h>
#include <qtooltip.h>
#include <qcursor.h>
#include <qevent.h>

// C headers
#include <cstring>
#include <cstdio>

#include "configure.h"
#include "statistics.h"

extern const char* programName;

KNetStats::KNetStats():mBRx(0), mBTx(0), mPRx(0), mPTx(0), mTotalBytesRx(0), mTotalBytesTx(0), mTotalPktRx(0), mTotalPktTx(0), mbConnected(true)
{
	// Load icons
	mIconBoth = loadIcon("icon_both.png");
	mIconRx = loadIcon("icon_rx.png");
	mIconTx = loadIcon("icon_tx.png");
	mIconNone = loadIcon("icon_none.png");
	mIconError = loadIcon("icon_error.png");
	mCurrentIcon = &mIconNone;
	setPixmap(*mCurrentIcon);

	// Context menu
	KPopupMenu* menu = contextMenu();

	KAction* configure = KStdAction::preferences(this, SLOT(configure()), actionCollection(), "configure");
	configure->plug(menu);

	menu->insertItem( i18n("&Statistics"), this, SLOT(statistics()) );

	menu->insertSeparator();
	KHelpMenu* helpmenu = new KHelpMenu(this, QString::null, false);
	menu->insertItem( i18n("&Help"), helpmenu->menu() );
	connect(helpmenu, SIGNAL(showAboutApplication()), this, SLOT(about()));

	// Load config
	KConfig cfg(KGlobal::dirs()->localkdedir()+"/share/config/knetstatsrc", true);
	mUpdateInterval = cfg.readNumEntry("UpdateInterval", 300);
	mInterface = cfg.readEntry("Interface");
	if (!mInterface.isEmpty())
		QToolTip::add(this, i18n("Monitoring ")+mInterface);

	// Quit signal
	connect(this, SIGNAL(quitSelected()), kapp, SLOT(quit(void)));

	// Timer
	mTimer = new QTimer(this, "timer");
	connect(mTimer, SIGNAL(timeout()), this, SLOT(update(void)));
	mTimer->start(mUpdateInterval);
}

void KNetStats::update()
{
	if (mInterface.isEmpty())
	{
		mTimer->stop();
		setPixmap( loadIcon("icon_configure.png") );
		if (KMessageBox::warningYesNo(this, "KNetstats appear to be not configured, do you want configure it now?") == KMessageBox::Yes)
			configure();
		return;
	}
	// Read and parse /proc/net/dev
	FILE* fp = fopen("/proc/net/dev", "r");
	if (!fp)
	{
		mTimer->stop();
		KMessageBox::error(this, "Error opening /proc/net/dev!");
		return;
	}

	unsigned int brx, btx, prx, ptx;
	char interface[8];
	char buffer[128];

	// skip headers (why not a fseek() ?)
	fgets(buffer, sizeof(buffer), fp);
	fgets(buffer, sizeof(buffer), fp);

	bool linkok = false;
	while(fgets(buffer, sizeof(buffer), fp))
	{
		// Search the interface string
		unsigned int i;
		for (i = 0; buffer[i]; i++)
		{
			if (buffer[i] != ' ')
			{
				for (unsigned int j = 0; j < (sizeof(interface)-1); j++,i++)
				{
					if (buffer[i] == ':')
					{
						interface[j] = 0;
						i++;
						break;
					}
					else
						interface[j] = buffer[i];
				}
				break;
			}
		}

		if (interface == mInterface)
		{
			QPixmap* newIcon;
			// Read statistics
			sscanf(&buffer[i], "%u%u%*u%*u%*u%*u%*u%*u%u%u%*u%*u%*u%*u%*u%*u", &brx, &prx, &btx, &ptx);

			if (brx == mBRx)
			{
				if (btx == mBTx )
					newIcon = &mIconNone;
				else
					newIcon = &mIconTx;
			}
			else
			{
				if (btx == mBTx )
					newIcon = &mIconRx;
				else
					newIcon = &mIconBoth;
			}

			mTotalBytesRx += brx - mBRx;
			mTotalBytesTx += btx - mBTx;
			mTotalPktRx += prx - mPRx;
			mTotalPktTx += ptx - mPTx;
			mBRx = brx;
			mBTx = btx;
			mPRx = prx;
			mPTx = ptx;

			if (newIcon != mCurrentIcon)
			{
				mCurrentIcon = newIcon;
				setPixmap(*mCurrentIcon);
			}
			if (!mbConnected)
			{
				mbConnected = true;
				KPassivePopup::message(programName, i18n("%1 is active").arg(mInterface), mIconBoth, this);
			}
			linkok = true;
			break;
		}
	}

	fclose(fp);

	if (!linkok && mbConnected)
	{
		mbConnected = false;
		mCurrentIcon = &mIconError;
		setPixmap(*mCurrentIcon);
		KPassivePopup::message(programName, i18n("%1 is inactive").arg(mInterface), mIconError, this);
	}

}

QStringList KNetStats::searchInterfaces()
{
	FILE* fp = fopen("/proc/net/dev", "r");

	QStringList list;

	if (!fp)
		return list;
	char interface[8];
	char buffer[128];

	fseek(fp, 199, SEEK_SET);
	while(fgets(buffer, sizeof(buffer), fp))
	{
		for( int i = 0; buffer[i]; i++)
		{
			if (buffer[i] != ' ')
			{
				for (int j = 0; buffer[i+j]; j++)
				{
					if (buffer[i+j] == ':')
					{
						interface[j] = 0;
						list.append(interface);
						break;
					}
					interface[j] = buffer[i+j];
				}
				break;
			}
		}

	}
	return list;
}

void KNetStats::configure()
{
	Configure dlg(this);
	int res = dlg.exec();
	if (res == QDialog::Accepted)
	{
		QString newInterface = dlg.interface();
		mUpdateInterval = dlg.updateInterval();
		mTimer->changeInterval(mUpdateInterval);

		if (newInterface != mInterface)
		{
			KConfig cfg(KGlobal::dirs()->localkdedir()+"/share/config/knetstatsrc");
			mInterface = newInterface;
			cfg.writeEntry("UpdateInterval", mUpdateInterval);
			cfg.writeEntry("Interface", mInterface);

			// Load statistics to the new interface
			cfg.setGroup(mInterface);
			mTotalBytesRx = mTotalBytesTx = mTotalPktRx = mTotalPktTx = 0;
			mBRx = mBTx = mPRx = mPTx = 0;

			QToolTip::add(this, i18n("Monitoring ")+mInterface);
		}
	}
}

void KNetStats::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::RightButton )
		contextMenu()->exec(QCursor::pos());
	else if (ev->button() == Qt::LeftButton)
		statistics();
}

void KNetStats::about()
{
	KAboutApplication dlg(this);
	dlg.exec();
}

void KNetStats::statistics()
{
	if (mInterface.isEmpty())
		configure();
	else
	{
		Statistics dlg(this);
		dlg.exec();
	}
}
