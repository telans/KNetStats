#include "knetstats.h"
#include "knetstatsview.h"
#include "configure.h"

#include <QMenu>
#include <QNetworkInterface>
#include <QSettings>
#include <QMessageBox>

extern const char *programName;

KNetStats::KNetStats() : QDialog(nullptr, Qt::Window), mConfigure(nullptr) {
	// read the current views from config file
	QSettings settings;
	QStringList views = settings.value("CurrentViews", QStringList()).toStringList();

	setupConfigure();
	setupBackupTrayIcon();
	if (views.empty()) {    // no views... =/, display the configuration dialog
		mConfigure->show();
	} else {
		// start the views
		for (auto &view: views) {
			auto *kview = new KNetStatsView(this, view);
			mViews[view] = kview;
		}
	}
}

void KNetStats::checkTrayIconsAvailable() {
	for (auto view: mViews) {
		if (view->trayIconVisible()) {
			mBackupTrayIcon->hide();
			return;
		}
	}
	mBackupTrayIcon->show();
}

void KNetStats::setupConfigure() {
	if (QNetworkInterface::allInterfaces().empty()) {
		QMessageBox msg(this);
		msg.setIcon(QMessageBox::Icon::Information);
		msg.setText("Error");
		msg.setInformativeText("Could not find any network interfaces!");
		msg.exec();
		mBackupTrayIcon->show();
		mBackupTrayIcon->showMessage(programName, QString("Could not find any network interfaces!"),
									 QSystemTrayIcon::Information,
									 3000);
	}

	mConfigure = new Configure(this);
	connect(mConfigure->mOk, &QPushButton::clicked, this, [this]() {
		configApply();
		mConfigure->hide();
	});
	connect(mConfigure->mApply, &QPushButton::clicked, this, &KNetStats::configApply);
	connect(mConfigure->mCancel, &QPushButton::clicked, this, [this]() { mConfigure->hide(); });
}

void KNetStats::setupBackupTrayIcon() {
	mBackupTrayIcon = new QSystemTrayIcon(QIcon(":/img/interfaces_missing.png"), this);
	mBackupTrayIcon->setToolTip("All Interfaces Unavailable");
	auto *mContextMenu = new QMenu(this);
	mContextMenu->addAction("Configure Interfaces", this, &KNetStats::showConfigure);
	mContextMenu->addAction("Quit KNetStats", this, []() { QApplication::quit(); });
	mBackupTrayIcon->setContextMenu(mContextMenu);

	connect(mBackupTrayIcon, &QSystemTrayIcon::activated, this, [this]() {
		if (mConfigure->isVisible()) {
			mConfigure->hide();
			return;
		}
		mConfigure->show();
	});
}

void KNetStats::readInterfaceConfig(const QString &ifName, ViewOptions *opts) {
	QSettings settings;
	int defaultTheme = ifName.startsWith("wlan") ? 3 : 0;

	settings.beginGroup(ifName);
	// General Settings
	opts->mUpdateInterval = settings.value("UpdateInterval", 300).toInt();
	opts->mMonitoring = settings.value("Monitoring", true).toBool();
	opts->mTheme = settings.value("Theme", defaultTheme).toInt();
	// Graph Settings
	opts->mChartUplColor = settings.value("ChartUplColor", "#FF0000").toString();
	opts->mChartDldColor = settings.value("ChartDldColor", "#00FF00").toString();
	opts->mChartBgColor = settings.value("ChartBgColor", "#000000").toString();
	opts->mChartTransparentBackground = settings.value("ChartUseTransparentBackground", false).toBool();
	settings.endGroup();
}

void KNetStats::configApply() {
	if (mConfigure->canSaveConfig())
		saveConfig(mConfigure->options());
}

void KNetStats::saveConfig(const OptionsMap &options) {
	QSettings settings;
	QStringList ifs;

	for (OptionsMap::ConstIterator i = options.begin(); i != options.end(); ++i) {
		TrayIconMap::Iterator trayIcon = mViews.find(i.key());
		const ViewOptions &opt = i.value();

		ifs.push_back(i.key());

		settings.beginGroup(i.key());
		// General Options
		settings.setValue("UpdateInterval", opt.mUpdateInterval);
		settings.setValue("Monitoring", opt.mMonitoring);
		settings.setValue("Theme", opt.mTheme);
		// Chart Options
		settings.setValue("ChartUplColor", opt.mChartUplColor);
		settings.setValue("ChartDldColor", opt.mChartDldColor);
		settings.setValue("ChartBgColor", opt.mChartBgColor);
		settings.setValue("ChartUseTransparentBackground", opt.mChartTransparentBackground);
		settings.endGroup();

		if (opt.mMonitoring) {    // check if we are already monitoring this interface.
			if (trayIcon == mViews.end()) { // new interface!
				auto *kview = new KNetStatsView(this, i.key());
				mViews[i.key()] = kview;
			} else
				trayIcon.value()->updateViewOptions();
		} else {
			// Check if a tray icon exist and remove then!
			if (trayIcon != mViews.end()) {
				delete trayIcon.value();
				mViews.erase(trayIcon);
			}
		}
		checkTrayIconsAvailable();
	}

	settings.setValue("AllViews", ifs);
	settings.setValue("CurrentViews", QStringList(mViews.keys()));
}
