#include "statistics.h"
#include "chart.h"
#include "knetstatsview.h"
#include <QNetworkInterface>
#include <QTimer>
#include <QMenu>

Statistics::Statistics(KNetStatsView *parent)
		: QDialog(parent), Ui::StatisticsBase(), mParent(parent) {

	setupUi(this);
	this->setWindowTitle(QString("Monitoring Interface %1 - KNetStats").arg(parent->mInterface));

	auto *chart = new Chart(parent->getViewOptions(), parent->mSpeedHistoryTx, parent->mSpeedHistoryRx,
							&parent->mMaxSpeed,
							&parent->mSpeedHistoryPtr, KNetStatsView::HISTORY_SIZE);
	mChart->addWidget(chart);
	this->update();

	mTimer = new QTimer(this);
	mTimer->setInterval(mParent->getViewOptions()->mUpdateInterval);
	connect(tabWidget, &QTabWidget::tabBarClicked, this, &Statistics::updateTabSize);
	connect(mTimer, &QTimer::timeout, this, &Statistics::updateStatistics);
	connect(mTimer, &QTimer::timeout, chart, qOverload<>(&Chart::repaint));
	connect(mOk, &QPushButton::clicked, this, &Statistics::hideWindow);
}

void Statistics::updateStatistics() {
	mMaxSpeed->setText(this->locale().formattedDataSize(mParent->mMaxSpeed) + +"/s");
	mBRx->setText(this->locale().formattedDataSize(mParent->mTotalBytesRx));
	mBTx->setText(this->locale().formattedDataSize(mParent->mTotalBytesTx));
	mByteSpeedRx->setText(this->locale().formattedDataSize(mParent->mSpeedHistoryRx[mParent->mSpeedHistoryPtr]) + "/s");
	mByteSpeedTx->setText(this->locale().formattedDataSize(mParent->mSpeedHistoryTx[mParent->mSpeedHistoryPtr]) + "/s");

	mPRx->setText(QString::number(mParent->mTotalPktRx));
	mPTx->setText(QString::number(mParent->mTotalPktTx));
	mPktSpeedRx->setText(QString::number(KNetStatsView::calcSpeed(mParent->mSpeedBufferPTx), 'f', 1) + "pkts /s");
	mPktSpeedTx->setText(QString::number(KNetStatsView::calcSpeed(mParent->mSpeedBufferPRx), 'f', 1) + "pkts /s");

	auto interface = QNetworkInterface::interfaceFromName(mParent->mInterface);
	mMTU->setNum(interface.maximumTransmissionUnit());
	mMAC->setText(interface.hardwareAddress());
	if (interface.flags() & QNetworkInterface::IsRunning) {
		QString ipStr, netmaskStr;
		for (const QNetworkAddressEntry &addr: interface.addressEntries()) {
			ipStr += addr.ip().toString() + "\n";
			netmaskStr += addr.netmask().toString() + "\n";
		}
		mIP->setText(ipStr.remove(QRegExp("\\n$")));
		mNetmask->setText(netmaskStr.remove(QRegExp("\\n$")));
		return;
	}
	mIP->setText("Not Connected");
	mNetmask->setText("Not Connected");
	if (!mParent->interfaceIsValid()) {
		mMTU->setText("N/A");
		mMAC->setText("N/A");
	}
}

void Statistics::updateTabSize(int tabIndex) {
	for (int i = 0; i < tabWidget->count(); i++) {
		if (i != tabIndex)
			tabWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	}

	tabWidget->widget(tabIndex)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	tabWidget->widget(tabIndex)->adjustSize();
}

void Statistics::showWindow() {
	mTimer->start(mParent->updateInterval());
	this->show();
}

void Statistics::hideWindow() {
	mTimer->stop();
	this->hide();
}
