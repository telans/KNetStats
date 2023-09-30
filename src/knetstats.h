#ifndef KNETSTATS_H
#define KNETSTATS_H

#include <set>
#include "configure.h"
#include <QSettings>
#include <QSystemTrayIcon>

class KNetStatsView;

class KNetStats : public QDialog {
Q_OBJECT
public:
	KNetStats();

	static void readInterfaceConfig(const QString &ifName, ViewOptions *opts);

	void checkTrayIconsAvailable();

public slots:

	void showConfigure() { mConfigure->show(); };

	/// Configure dialog Apply button
	void configApply();

private:
	typedef QHash<QString, KNetStatsView *> TrayIconMap;
	QSystemTrayIcon *mBackupTrayIcon;
	TrayIconMap mViews;
	Configure *mConfigure;

	void setupConfigure();

	void setupBackupTrayIcon();

	void saveConfig(const OptionsMap &options);
};

#endif
