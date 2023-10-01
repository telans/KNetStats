#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QString>
#include <QMap>
#include <QFont>
#include <QColor>
#include <QNetworkInterface>

#include "ui_configurebase.h"

struct ViewOptions {
	// general
	int mUpdateInterval;
	bool mMonitoring;
	bool mNotifications;
	// icon view
	int mTheme;
	// chart view
	QString mChartUplColor;
	QString mChartDldColor;
	QString mChartBgColor;
	bool mChartTransparentBackground;
};

typedef QMap<QString, ViewOptions> OptionsMap;

class Configure : public QDialog, public Ui::ConfigureBase {
Q_OBJECT
public:
	explicit Configure(QWidget *parent);

	bool canSaveConfig();

	const OptionsMap &options() const { return mConfig; }

protected slots:

	void changeInterface(QListWidgetItem *item);

	void changeTheme(int theme);

private:
	QString mCurrentItem;
	OptionsMap mConfig;
	QIcon *mInterfaceIcon;

private slots:

	void refreshInterfaces();
};

#endif
