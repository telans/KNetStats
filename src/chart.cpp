#include "chart.h"
#include <QPainter>
#include <QBrush>
#include "configure.h"

Chart::Chart(const double *uploadBuffer, const double *downloadBuffer, int bufferSize, const int *ptr,
			 const double *maxspeed, const ViewOptions *interfaceOptions)
		: QWidget(nullptr), mUplBuffer(uploadBuffer), mDldBuffer(downloadBuffer), mBufferSize(bufferSize), mPtr(ptr),
		  mMaxSpeed(maxspeed), mInterfaceOptions(interfaceOptions) {}

void Chart::paintEvent(QPaintEvent *event) {
	QPainter paint(this);
	paint.setBackground(QColor(mInterfaceOptions->mChartBgColor));
	paint.setBackgroundMode(mInterfaceOptions->mChartTransparentBackground ? Qt::TransparentMode : Qt::OpaqueMode);
	QBrush brush(QColor(0x33, 0x33, 0x33), Qt::BrushStyle::CrossPattern);
	paint.fillRect(0, 0, width(), height(), brush);

	const double step = width() / double(mBufferSize);
	const int HEIGHT = height() - 1;

	int x;
	int lastX = x = width();
	int lastRxY = HEIGHT - int(HEIGHT * (mDldBuffer[*mPtr] / (*mMaxSpeed)));
	int lastTxY = HEIGHT - int(HEIGHT * (mUplBuffer[*mPtr] / (*mMaxSpeed)));

	int count = 0;
	for (int i = *mPtr; count < mBufferSize; i--) {
		if (i < 0)
			i = mBufferSize - 1;
		int rxY = HEIGHT - int(HEIGHT * (mDldBuffer[i] / (*mMaxSpeed)));
		int txY = HEIGHT - int(HEIGHT * (mUplBuffer[i] / (*mMaxSpeed)));
		paint.setPen(mInterfaceOptions->mChartDldColor);
		paint.drawLine(lastX, lastRxY, x, rxY);
		paint.setPen(mInterfaceOptions->mChartUplColor);
		paint.drawLine(lastX, lastTxY, x, txY);

		lastX = x;
		lastRxY = rxY;
		lastTxY = txY;

		count++;
		x = width() - int(step * (count + 1));
	}
}

