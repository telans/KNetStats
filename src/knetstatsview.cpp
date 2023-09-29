#include "knetstatsview.h"
#include "knetstats.h"
#include <QTimer>
#include <qevent.h>
#include <qpainter.h>

#include <QMenu>
#include "statistics.h"

extern const char *programName;

KNetStatsView::KNetStatsView(KNetStats *parent, const QString &interface)
		: mSysDevPath("/sys/class/net/" + interface + "/"),
		  mInterface(QNetworkInterface::interfaceFromName(interface)) {
	mFirstUpdate = true;
	mCarrier = true;

	mTimer = new QTimer(this);
	mStatistics = new Statistics(this);
	trayIcon = new QSystemTrayIcon(this);

	mContextMenu = new QMenu(this);
	mContextMenu->addAction("Configure Interfaces", parent, &KNetStats::configure);
	mContextMenu->addAction("Quit KNetStats", this, []() {
		QApplication::quit();
	});

	KNetStats::readInterfaceConfig(interface, &mOptions);
	resetBuffers();
	setupTrayIcon();

	trayIcon->setToolTip(QString("Monitoring %1").arg(mInterface.name()));
	trayIcon->setContextMenu(mContextMenu);
	connect(mTimer, &QTimer::timeout, this, &KNetStatsView::updateStats);
	connect(trayIcon, &QSystemTrayIcon::activated, this, &KNetStatsView::iconActivated);
}

void KNetStatsView::setupTrayIcon() {
	if (mOptions.mViewMode == Text)
		mOptions.mTxtFont.setFamily(mOptions.mTxtFont.defaultFamily());
	else if (mOptions.mViewMode == Icon) {
		// Load Icons
		mIconError = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_error.png");
		mIconNone = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_none.png");
		mIconTx = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_tx.png");
		mIconRx = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_rx.png");
		mIconBoth = QIcon(":/img/theme" + QString::number(mOptions.mTheme) + "_both.png");
		mCurrentIcon = &mIconNone;
	}
	mTimer->start(mOptions.mUpdateInterval);
	trayIcon->setIcon(*mCurrentIcon);
	trayIcon->show();
}

void KNetStatsView::updateViewOptions() {
	KNetStats::readInterfaceConfig(mInterface.name(), &mOptions);
	setupTrayIcon();
}

void KNetStatsView::updateStats() {
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
			trayIcon->showMessage(programName, QString("%1 is down!").arg(mInterface.name()),
								  QSystemTrayIcon::Information, 3000);
			QApplication::processEvents();
			trayIcon->hide();
		}
		return;
	} else if (!mCarrier) { // carrier up
		mCarrier = true;
		trayIcon->show();
		trayIcon->showMessage(programName, QString("%1 is up!").arg(mInterface.name()),
							  QSystemTrayIcon::Information, 3000);
		QApplication::processEvents();
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

	if (mOptions.mViewMode == Icon) {
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
		}
	} else if (mOptions.mViewMode == Graphic || (btx != mBTx && brx != mBRx)) {
//		updateTrayIconAndStats();
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
	// stdio functions appear to be more fast than QFile?
	FILE *fp = fopen((mSysDevPath + "statistics/" + name).toLatin1(), "r");
	unsigned long long retval;
	fscanf(fp, "%llu", &retval);
	fclose(fp);
	return retval;
}

void KNetStatsView::resetBuffers() {
	memset(mSpeedHistoryRx, 0, sizeof(double) * HISTORY_SIZE);
	memset(mSpeedHistoryTx, 0, sizeof(double) * HISTORY_SIZE);
	memset(mSpeedBufferRx, 0, sizeof(double) * SPEED_BUFFER_SIZE);
	memset(mSpeedBufferTx, 0, sizeof(double) * SPEED_BUFFER_SIZE);
	memset(mSpeedBufferPRx, 0, sizeof(double) * SPEED_BUFFER_SIZE);
	memset(mSpeedBufferPTx, 0, sizeof(double) * SPEED_BUFFER_SIZE);
}
//
//void KNetStatsView::paintEvent(QPaintEvent *ev) {
//	QPainter paint(this);
//	switch (mOptions.mViewMode) {
//		case Icon:
//			if (!mCarrier || !mConnected)
//				mCurrentIcon = &mIconError;
//			paint.drawPixmap(0, 0, mCurrentIcon->grab());
//			break;
//		case Text:
//			drawText(paint);
//			break;
//		case Graphic:
//			drawGraphic(paint);
//			break;
//	}
//}

//void KNetStatsView::drawText(QPainter &paint) {
//	if (!mCarrier || !mConnected) {
//		paint.drawText(rect(), Qt::AlignCenter, "?");
//	} else {
//		paint.setFont(mOptions.mTxtFont);
//		paint.setPen(mOptions.mTxtUplColor);
//		paint.drawText(rect(), Qt::AlignTop, this->locale().formattedDataSize(byteSpeedTx()) + "/s");
//		paint.setPen(mOptions.mTxtDldColor);
//		paint.drawText(rect(), Qt::AlignBottom, this->locale().formattedDataSize(byteSpeedRx()) + "/s");
//	}
//}
//
//void KNetStatsView::drawGraphic(QPainter &paint) {
//	if (!mCarrier || !mConnected) {
//		paint.drawText(rect(), Qt::AlignCenter, "X");
//		return;
//	}
//
//	QSize size = this->size();
//
//	if (!mOptions.mChartTransparentBackground)
//		paint.fillRect(0, 0, size.width(), size.height(), mOptions.mChartBgColor);
//
//	const int HEIGHT = size.height() - 1;
//
//	//	qDebug("MaxSpeed: %d, age: %d", int(mMaxSpeed), mMaxSpeedAge);
//	int lastX;
//	int lastRxY = HEIGHT - int(HEIGHT * (mSpeedHistoryRx[mSpeedHistoryPtr] / mMaxSpeed));
//	int lastTxY = HEIGHT - int(HEIGHT * (mSpeedHistoryTx[mSpeedHistoryPtr] / mMaxSpeed));
//	int x = lastX = size.width();
//	int count = 0;
//	for (int i = mSpeedHistoryPtr; count < width(); i--) {
//		if (i < 0)
//			i = HISTORY_SIZE - 1;
//
//		int rxY = HEIGHT - int(HEIGHT * (mSpeedHistoryRx[i] / mMaxSpeed));
//		int txY = HEIGHT - int(HEIGHT * (mSpeedHistoryTx[i] / mMaxSpeed));
//		paint.setPen(mOptions.mChartDldColor);
//		paint.drawLine(lastX, lastRxY, x, rxY);
//		paint.setPen(mOptions.mChartUplColor);
//		paint.drawLine(lastX, lastTxY, x, txY);
//		//qDebug("%d => %d", i, int(mSpeedHistoryRx[i]));
//		lastX = x;
//		lastRxY = rxY;
//		lastTxY = txY;
//
//		count++;
//		x = width() - int(count + 1);
//	}
//}

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
