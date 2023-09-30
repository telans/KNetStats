#ifndef KNETSTATS_H
#define KNETSTATS_H

#include <set>
#include "configure.h"
#include <QSettings>

class KNetStatsView;

class KNetStats : public QDialog {
Q_OBJECT
public:
	KNetStats();

	static void readInterfaceConfig(const QString &ifName, ViewOptions *opts);

	/**
	*	Exists at least 1 view?
	*/
	bool canStart() const {
		return mCanStart;
	}

public slots:

	/// Display configure the dialog box
	bool configure();

	/// Configure dialog OK button
	void configOk();

	/// Configure dialog Apply button
	void configApply();

	/// Configure dialog Cancel button
	void configCancel();

private:
	Configure *mConfigure;
	typedef QHash<QString, KNetStatsView *> TrayIconMap;
	TrayIconMap mViews;
	bool mCanStart;

	void saveConfig(const OptionsMap &options);
};

#endif
