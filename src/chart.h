#ifndef CHART_H
#define CHART_H

#include <qwidget.h>
#include "configure.h"

class QPaintEvent;

class Chart : public QWidget {
Q_OBJECT
public:
	Chart(const ViewOptions *interfaceOptions, const double *uploadBuffer, const double *downloadBuffer,
		  const double *maxspeed, const int *ptr, int bufferSize) : QWidget() {
		mUplBuffer = uploadBuffer;
		mDldBuffer = downloadBuffer;
		mBufferSize = bufferSize;
		mPtr = ptr;
		mMaxSpeed = maxspeed;
		mInterfaceOptions = interfaceOptions;
	};

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	const ViewOptions *mInterfaceOptions;
	const double *mUplBuffer;
	const double *mDldBuffer;
	const double *mMaxSpeed;
	const int *mPtr;
	int mBufferSize;
};

#endif
