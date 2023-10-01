#include "knetstatsview.h"
#include "knetstats.h"
#include <QTimer>
#include <qevent.h>
#include <qpainter.h>

#include <QMenu>
#include <fstream>
#include "statistics.h"

extern const char *programName;

KNetStatsView::KNetStatsView(KNetStats *parent, const QString &interface)
		: mParent(parent), mSysDevPath("/sys/class/net/" + interface + "/") {
	mInterface = interface;
	mCarrier = interfaceIsValid();
	mFirstUpdate = true;

	mTimer = new QTimer(this);
	mStatistics = new Statistics(this);
	mTrayIcon = new QSystemTrayIcon(this);
	mContextMenu = new QMenu(this);
	mContextMenu->addAction("Configure Interfaces", parent, &KNetStats::showConfigure);
	mContextMenu->addAction("Quit KNetStats", parent, []() { QApplication::quit(); });

	KNetStats::readInterfaceConfig(interface, &mOptions);
	setupTrayIcon();
	setupView();

	mTimer->start(mOptions.mUpdateInterval);
	connect(mTrayIcon, &QSystemTrayIcon::activated, this, &KNetStatsView::iconActivated);
}

void KNetStatsView::setupView() {
	if (!interfaceIsValid()) {
		connect(mTimer, &QTimer::timeout, this, &KNetStatsView::checkMissingInterface);
		return;
	}

	mTrayIcon->show();
	connect(mTimer, &QTimer::timeout, this, &KNetStatsView::updateStats);
}

void KNetStatsView::setupTrayIcon() {
	// Load Icons
	mIconError = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_error.png");
	mIconNone = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_none.png");
	mIconTx = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_tx.png");
	mIconRx = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_rx.png");
	mIconBoth = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_both.png");
	mCurrentIcon = &mIconNone;
	mTrayIcon->setToolTip(QString("Monitoring %1").arg(mInterface));
	mTrayIcon->setContextMenu(mContextMenu);
	mTrayIcon->setIcon(*mCurrentIcon);
}

void KNetStatsView::checkMissingInterface() {
	if (interfaceIsValid()) {
		mTrayIcon->show();
		if (mOptions.mNotifications)
			mTrayIcon->showMessage(programName, QString("Interface %1 reappeared!").arg(mInterface),
								   QSystemTrayIcon::Information,
								   3000);
		disconnect(mTimer, &QTimer::timeout, this, &KNetStatsView::checkMissingInterface);
		connect(mTimer, &QTimer::timeout, this, &KNetStatsView::updateStats);
	}
	mParent->checkTrayIconsAvailable();
}

void KNetStatsView::interfaceMissing() {
	if (mOptions.mNotifications)
		mTrayIcon->showMessage(programName, QString("Interface %1 disappeared!").arg(mInterface),
							   QSystemTrayIcon::Information,
							   3000);
	QApplication::processEvents();
	mTrayIcon->hide();
	disconnect(mTimer, &QTimer::timeout, this, &KNetStatsView::updateStats);
	connect(mTimer, &QTimer::timeout, this, &KNetStatsView::checkMissingInterface);
	mParent->checkTrayIconsAvailable();
}

void KNetStatsView::updateViewOptions() {
	KNetStats::readInterfaceConfig(mInterface, &mOptions);
	setupTrayIcon();
}

void KNetStatsView::updateStats() {
	if (!interfaceIsValid()) {
		interfaceMissing();
		return;
	}

	FILE *fp = fopen((mSysDevPath + "carrier").toLatin1(), "r");
	int carrierFlag = 0;

	if (fp) {
		carrierFlag = fgetc(fp);
		// /sys/net/<>/carrier can immediately read EOF if the network state is DOWN. Pin it to 0.
		carrierFlag = (carrierFlag < 0) ? '0' : carrierFlag;
		fclose(fp);
	}

	if (carrierFlag == '0') { // carrier down
		if (mCarrier) {
			mCarrier = false;
			if (mOptions.mNotifications)
				mTrayIcon->showMessage(programName, QString("Interface %1 is down!").arg(mInterface),
									   QSystemTrayIcon::Information,
									   3000);
			QApplication::processEvents();
			mTrayIcon->hide();
			mParent->checkTrayIconsAvailable();
		}
		return;
	} else if (!mCarrier) { // carrier up
		mCarrier = true;
		mTrayIcon->show();
		if (mOptions.mNotifications)
			mTrayIcon->showMessage(programName, QString("Interface %1 is up!").arg(mInterface),
								   QSystemTrayIcon::Information,
								   3000);
		mParent->checkTrayIconsAvailable();
	}

	unsigned long long brx = readInterfaceNumValue("rx_bytes");
	unsigned long long btx = readInterfaceNumValue("tx_bytes");
	unsigned long long prx = readInterfaceNumValue("rx_packets");
	unsigned long long ptx = readInterfaceNumValue("tx_packets");

	if (!mFirstUpdate) { // a primeira velocidade sempre eh absurda, para evitar isso temos o mFirstUpdate
		if (++mSpeedBufferPtr == SPEED_BUFFER_SIZE)
			mSpeedBufferPtr = 0;
		if (++mSpeedHistoryPtr == HISTORY_SIZE)
			mSpeedHistoryPtr = 0;

		// Calcula as velocidades
		mSpeedBufferTx[mSpeedBufferPtr] = ((btx - mBTx) * (1000.0f / mOptions.mUpdateInterval));
		mSpeedBufferRx[mSpeedBufferPtr] = ((brx - mBRx) * (1000.0f / mOptions.mUpdateInterval));
		mSpeedBufferPTx[mSpeedBufferPtr] = ((ptx - mPTx) * (1000.0f / mOptions.mUpdateInterval));
		mSpeedBufferPRx[mSpeedBufferPtr] = ((prx - mPRx) * (1000.0f / mOptions.mUpdateInterval));

		mSpeedHistoryRx[mSpeedHistoryPtr] = calcSpeed(mSpeedBufferRx);
		mSpeedHistoryTx[mSpeedHistoryPtr] = calcSpeed(mSpeedBufferTx);

		mMaxSpeedAge--;

		if (mSpeedHistoryTx[mSpeedHistoryPtr] > mMaxSpeed) {
			mMaxSpeed = mSpeedHistoryTx[mSpeedHistoryPtr];
			mMaxSpeedAge = HISTORY_SIZE;
		}
		if (mSpeedHistoryRx[mSpeedHistoryPtr] > mMaxSpeed) {
			mMaxSpeed = mSpeedHistoryRx[mSpeedHistoryPtr];
			mMaxSpeedAge = HISTORY_SIZE;
		}
		if (mMaxSpeedAge < 1)
			calcMaxSpeed();
	} else {
		mFirstUpdate = false;
	}

	QIcon *newIcon;
	if (brx == mBRx) {
		if (btx == mBTx)
			newIcon = &mIconNone;
		else
			newIcon = &mIconTx;
	} else {
		if (btx == mBTx)
			newIcon = &mIconRx;
		else
			newIcon = &mIconBoth;
	}

	if (newIcon != mCurrentIcon) {
		mCurrentIcon = newIcon;
		mTrayIcon->setIcon(*mCurrentIcon);
	}

	// Update stats
	mTotalBytesRx += brx - mBRx;
	mTotalBytesTx += btx - mBTx;
	mTotalPktRx += prx - mPRx;
	mTotalPktTx += ptx - mPTx;

	mBRx = brx;
	mBTx = btx;
	mPRx = prx;
	mPTx = ptx;
}

unsigned long long KNetStatsView::readInterfaceNumValue(const char *name) {
	unsigned long long retval;
	std::ifstream file((mSysDevPath + "statistics/" + name).toLatin1());
	file >> retval;
	return retval;
}

void KNetStatsView::iconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
		if (mStatistics->isVisible())
			mStatistics->hideWindow();
		else
			mStatistics->showWindow();
	} else if (reason == QSystemTrayIcon::ActivationReason::Context) {
		mContextMenu->exec();
	}
}
