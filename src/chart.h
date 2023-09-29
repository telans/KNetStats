#ifndef CHART_H
#define CHART_H

#include <qwidget.h>

class QPaintEvent;

class Chart : public QWidget {
	Q_OBJECT
public:
	Chart(const double *uploadBuffer, const double *downloadBuffer, int bufferSize, const int *ptr,
		  const double *maxspeed, const QString *uploadColor, const QString *downloadColor);

protected:
	void paintEvent(QPaintEvent *event);

private:
	const double *mUplBuffer;
	const double *mDldBuffer;
	const int mBufferSize;
	const int *mPtr;
	const double *mMaxSpeed;
	const QString *mChartUplColor;
	const QString *mChartDldColor;
};

#endif
