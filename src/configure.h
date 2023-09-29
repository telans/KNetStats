#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QString>
#include <QMap>
#include <QFont>
#include <QColor>
#include <QNetworkInterface>

#include "ui_configurebase.h"

enum ViewMode {
	Icon, Text, Graphic
};

struct ViewOptions {
	// general
	int mUpdateInterval;
	ViewMode mViewMode;
	bool mMonitoring;
	// txt view
//	QFont mTxtFont;
	QString mTxtUplColor;
	QString mTxtDldColor;
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
	explicit Configure(QWidget *parent, const QList<QNetworkInterface> &ifs);

	bool canSaveConfig();

	const OptionsMap &options() const { return mConfig; }

protected slots:

	void changeInterface(QListWidgetItem *item);

	void changeTheme(int theme);

	void openColorDialog();

private:
	QString mCurrentItem;
	OptionsMap mConfig;

};

#endif
