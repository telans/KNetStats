#include "knetstats.h"
#include "knetstatsview.h"
#include "configure.h"

#include <QMenu>
#include <QNetworkInterface>
#include <QSettings>
#include <QMessageBox>

KNetStats::KNetStats() : QDialog(nullptr, Qt::Window), mCanStart(true), mConfigure(nullptr) {
	// read the current views from config file
	QSettings settings;
	QStringList views = settings.value("CurrentViews", QStringList()).toStringList();

	if (views.empty()) {    // no views... =/, display the configuration dialog
		if (!this->configure())
			mCanStart = false;
	} else {
		// start the views
		for (auto &view: views) {
			auto *kview = new KNetStatsView(this, view);
			mViews[view] = kview;
		}
	}
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

bool KNetStats::configure() {
	if (mConfigure)
		mConfigure->show();
	else {
		QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();

		if (ifs.empty()) {
			QMessageBox msg(this);
			msg.setIcon(QMessageBox::Icon::Information);
			msg.setText("Error");
			msg.setInformativeText("Could not find any network interfaces.\nKNetStats will quit now.");
			msg.exec();
			return false;
		}

		mConfigure = new Configure(this, ifs);
		connect(mConfigure->mOk, &QPushButton::clicked, this, &KNetStats::configOk);
		connect(mConfigure->mApply, &QPushButton::clicked, this, &KNetStats::configApply);
		connect(mConfigure->mCancel, &QPushButton::clicked, this, &KNetStats::configCancel);
		mConfigure->show();
	}
	return true;
}

void KNetStats::configOk() {
	if (mConfigure->canSaveConfig()) {
		saveConfig(mConfigure->options());
		delete mConfigure;
		mConfigure = nullptr;
	}
}

void KNetStats::configApply() {
	if (mConfigure->canSaveConfig())
		saveConfig(mConfigure->options());
}

void KNetStats::configCancel() {
	delete mConfigure;
	mConfigure = nullptr;

	if (mViews.empty())
		QApplication::quit();
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
	}

	settings.setValue("AllViews", ifs);
	settings.setValue("CurrentViews", QStringList(mViews.keys()));
}
