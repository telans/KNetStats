#ifndef STATISTICS_H
#define STATISTICS_H

#include "ui_statisticsbase.h"
#include <qstring.h>
#include <QNetworkInterface>

class KNetStatsView;

class Statistics : public QDialog, public Ui::StatisticsBase {
Q_OBJECT
public:
	explicit Statistics(KNetStatsView *parent);

private:
	QTimer *mTimer;
	KNetStatsView *mParent;

public slots:

	void showWindow();

	void hideWindow();

private slots:

	void updateStatistics();
};

#endif
