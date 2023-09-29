#include <QListWidget>
#include <QNetworkInterface>
#include <QMessageBox>

#include "configure.h"
#include "knetstats.h"

#include "ui_configurebase.h"

Configure::Configure(QWidget *parent, const QList<QNetworkInterface> &ifs) : QDialog(parent), Ui::ConfigureBase() {
	setupUi(this);
	QIcon iconPCI = QIcon(":/img/icon_pci.png");
	mInterfaces->setViewMode(QListWidget::ListMode);
	// Clone the configuration.
	for (auto &it: ifs) {
		auto *item = new QListWidgetItem(iconPCI, it.name());
		mInterfaces->insertItem(it.index(), item);
		KNetStats::readInterfaceConfig(it.name(), &mConfig[it.name()]);
	}

	mInterfaces->setCurrentRow(0);
	changeInterface(mInterfaces->currentItem());

	connect(mInterfaces, SIGNAL(itemClicked(QListWidgetItem * )), this, SLOT(changeInterface(QListWidgetItem * )));
	connect(mTheme, SIGNAL(activated(int)), this, SLOT(changeTheme(int)));
}

void Configure::changeInterface(QListWidgetItem *item) {
	QString interface = item->text();

	if (!mCurrentItem.isEmpty()) {
		// Save the previous options
		ViewOptions &oldview = mConfig[mCurrentItem];
		// general options
		oldview.mMonitoring = mMonitoringInterface->isChecked();
		oldview.mUpdateInterval = mUpdateInterval->value();
		oldview.mViewMode = (ViewMode) mViewMode->currentIndex();
		// txt view options
		oldview.mTxtUplColor = mTxtUplColor->color().name();
		oldview.mTxtDldColor = mTxtDldColor->color().name();
		oldview.mTxtFont = mTxtFont->font();
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
	// general
	mMonitoringInterface->setChecked(view.mMonitoring);
	mUpdateInterval->setValue(view.mUpdateInterval);
	mViewMode->setCurrentIndex(view.mViewMode);
	// txt options
	mTxtUplColor->setColor(view.mTxtUplColor);
	mTxtDldColor->setColor(view.mTxtDldColor);
	mTxtFont->setFont(view.mTxtFont);
	// icon options
	mTheme->setCurrentIndex(view.mTheme);
	changeTheme(view.mTheme);
	// chart options
	mChartUplColor->setColor(view.mChartUplColor);
	mChartDldColor->setColor(view.mChartDldColor);
	mChartBgColor->setColor(view.mChartBgColor);
	mChartTransparentBackground->setChecked(view.mChartTransparentBackground);

	mCurrentItem = interface;
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
