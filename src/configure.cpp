#include <QListWidget>
#include <QNetworkInterface>
#include <QMessageBox>

#include "configure.h"
#include "knetstats.h"

#include "ui_configurebase.h"

Configure::Configure(QWidget *parent) : QDialog(parent), Ui::ConfigureBase() {
	setupUi(this);
	mInterfaceIcon = new QIcon(":/img/icon_pci.png");
	mInterfaces->setViewMode(QListWidget::ListMode);

	refreshInterfaces();
	connect(this->mRefreshBtn, &QPushButton::clicked, this, &Configure::refreshInterfaces);
	connect(mInterfaces, &QListWidget::itemClicked, this, &Configure::changeInterface);
	connect(mTheme, qOverload<int>(&QComboBox::activated), this, &Configure::changeTheme);
}

void Configure::refreshInterfaces() {
	int currentRow = mInterfaces->currentRow();

	while (mInterfaces->count() > 0) {
		auto interface = mInterfaces->takeItem(0);
		delete interface;
	}

	auto interfaces = QNetworkInterface::allInterfaces();
	for (auto &it: interfaces) {
		auto *item = new QListWidgetItem(*mInterfaceIcon, it.name());
		mInterfaces->insertItem(it.index(), item);
		KNetStats::readInterfaceConfig(it.name(), &mConfig[it.name()]);
	}

	auto *item = mInterfaces->item(currentRow);
	if (!item)
		item = mInterfaces->item(0);
	mInterfaces->setCurrentItem(item);
	changeInterface(item);
}

void Configure::changeInterface(QListWidgetItem *item) {
	if (!item)
		return;
	QString interface = item->text();

	if (!mCurrentItem.isEmpty()) {
		// Save the previous options
		ViewOptions &oldview = mConfig[mCurrentItem];
		// general options
		oldview.mMonitoring = mMonitoringInterface->isChecked();
		oldview.mNotifications = mDisplayNotifications->isChecked();
		oldview.mUpdateInterval = mUpdateInterval->value();
		// icon view
		oldview.mTheme = mTheme->currentIndex();
		// chart view
		oldview.mChartUplColor = mChartUplColor->color().name();
		oldview.mChartDldColor = mChartDldColor->color().name();
		oldview.mChartBgColor = mChartBgColor->color().name();
		oldview.mChartTransparentBackground = mChartTransparentBackground->isChecked();
	}

	if (interface == mCurrentItem)
		return;
	// Load the new interface options
	ViewOptions &view = mConfig[interface];
	// General options
	mMonitoringInterface->setChecked(view.mMonitoring);
	mDisplayNotifications->setChecked(view.mNotifications);
	mUpdateInterval->setValue(view.mUpdateInterval);
	mTheme->setCurrentIndex(view.mTheme);
	// Chart Options
	mChartUplColor->setColor(view.mChartUplColor);
	mChartDldColor->setColor(view.mChartDldColor);
	mChartBgColor->setColor(view.mChartBgColor);
	mChartTransparentBackground->setChecked(view.mChartTransparentBackground);
	mCurrentItem = interface;

	changeTheme(view.mTheme);
}

bool Configure::canSaveConfig() {
	// update the options
	changeInterface(mInterfaces->item(mInterfaces->currentRow()));

	bool ok = false;
	for (OptionsMap::ConstIterator i = mConfig.begin(); i != mConfig.end(); ++i)
		if (i.value().mMonitoring) {
			ok = true;
			break;
		}

	if (!ok) {
		QMessageBox msg(this);
		msg.setIcon(QMessageBox::Icon::Information);
		msg.setText("Error");
		msg.setInformativeText("You need to select at least one interface to monitor.");
		msg.exec();
	}

	return ok;
}

void Configure::changeTheme(int theme) {
	mIconError->setPixmap(QPixmap(":/img/theme" + QString::number(theme) + "_error.png"));
	mIconNone->setPixmap(QPixmap(":/img/theme" + QString::number(theme) + "_none.png"));
	mIconTx->setPixmap(QPixmap(":/img/theme" + QString::number(theme) + "_tx.png"));
	mIconRx->setPixmap(QPixmap(":/img/theme" + QString::number(theme) + "_rx.png"));
	mIconBoth->setPixmap(QPixmap(":/img/theme" + QString::number(theme) + "_both.png"));
}
