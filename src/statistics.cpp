#include "statistics.h"
#include "chart.h"
#include "knetstatsview.h"
#include <QTimer>
#include <QMenu>

Statistics::Statistics(KNetStatsView *parent)
		: Ui::StatisticsBase(), mParent(parent) {
//	setCaption( i18n( "Details of %1" ).arg( mInterface ) );

	setupUi(this);
	auto *chart = new Chart(parent->speedHistoryTx(), parent->speedHistoryRx(), parent->historyBufferSize(),
							parent->historyPointer(), parent->maxSpeed(),
							parent->getViewOptions());
	mChart->addWidget(chart);
	update();

	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
	connect(mTimer, SIGNAL(timeout()), chart, SLOT(update()));

	connect(mOk, SIGNAL(clicked(bool)), this, SLOT(accept()));
}

void Statistics::update() {
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

void Statistics::show() {
	// Update details...
	mMTU->setNum(mParent->interface().maximumTransmissionUnit());
	mIP->setText(mParent->interface().addressEntries()[0].ip().toString());
	mNetmask->setText(mParent->interface().addressEntries()[0].netmask().toString());
	mMAC->setText(mParent->interface().hardwareAddress());

	mTimer->start(mParent->updateInterval());
	QWidget::show();
}

void Statistics::accept() {
	mTimer->stop();
	QDialog::accept();
}
