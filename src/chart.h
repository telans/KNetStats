#ifndef CHART_H
#define CHART_H

#include <qwidget.h>
#include "configure.h"

class QPaintEvent;

class Chart : public QWidget {
	Q_OBJECT
public:
	Chart(const double *uploadBuffer, const double *downloadBuffer, int bufferSize, const int *ptr,
		  const double *maxspeed, const ViewOptions *interfaceOptions);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	const double *mUplBuffer;
	const double *mDldBuffer;
	const int mBufferSize;
	const int *mPtr;
	const double *mMaxSpeed;
	const ViewOptions *mInterfaceOptions;
};

#endif
