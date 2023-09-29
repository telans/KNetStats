#include "statistics.h"
#include "chart.h"
#include "knetstatsview.h"
#include <QNetworkInterface>
#include <QTimer>
#include <QMenu>

Statistics::Statistics(KNetStatsView *parent)
		: Ui::StatisticsBase(), mParent(parent) {

	setupUi(this);
	this->setWindowTitle(QString("Monitoring Interface %1 - KNetStats").arg(parent->interface().name()));
	auto *chart = new Chart(parent->speedHistoryTx(), parent->speedHistoryRx(), parent->historyBufferSize(),
							parent->historyPointer(), parent->maxSpeed(),
							parent->getViewOptions());
	mChart->addWidget(chart);
	update();

	mTimer = new QTimer(this);
	connect(mTimer, &QTimer::timeout, this, &Statistics::updateStatistics);
	connect(mTimer, &QTimer::timeout, chart, qOverload<>(&Chart::repaint));
	connect(mOk, &QPushButton::clicked, this, &Statistics::hideWindow);
}

void Statistics::updateStatistics() {
	mMaxSpeed->setText(this->locale().formattedDataSize(*mParent->maxSpeed()) + +"/s");
	mBRx->setText(this->locale().formattedDataSize(mParent->totalBytesRx()));
	mBTx->setText(this->locale().formattedDataSize(mParent->totalBytesTx()));
	mByteSpeedRx->setText(this->locale().formattedDataSize(mParent->byteSpeedRx()) + "/s");
	mByteSpeedTx->setText(this->locale().formattedDataSize(mParent->byteSpeedTx()) + "/s");

	mPRx->setText(QString::number(mParent->totalPktRx()));
	mPTx->setText(QString::number(mParent->totalPktTx()));
	mPktSpeedRx->setText(QString::number(mParent->pktSpeedRx(), 'f', 1) + "pkts/s");
	mPktSpeedTx->setText(QString::number(mParent->pktSpeedTx(), 'f', 1) + "pkts/s");
}

void Statistics::showWindow() {
	// Update details...
	mMTU->setNum(mParent->interface().maximumTransmissionUnit());
	if (mParent->interface().flags() & (QNetworkInterface::InterfaceFlag::IsUp | QNetworkInterface::InterfaceFlag::IsRunning)) {
		mIP->setText(mParent->interface().addressEntries()[0].ip().toString());
		mNetmask->setText(mParent->interface().addressEntries()[0].netmask().toString());
	} else {
		mIP->setText("Not Connected");
		mNetmask->setText("Not Connected");
	}
	mMAC->setText(mParent->interface().hardwareAddress());

	mTimer->start(mParent->updateInterval());
	show();
}

void Statistics::hideWindow() {
	mTimer->stop();
	hide();
}
