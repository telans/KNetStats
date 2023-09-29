#ifndef KNETSTATSVIEW_H
#define KNETSTATSVIEW_H

#include <arpa/inet.h>
#include <linux/netdevice.h>
#include <QSystemTrayIcon>
#include "configure.h"
#include "knetstats.h"
#include "statistics.h"

class KNetStatsView : public QWidget {
Q_OBJECT

	enum {
		HISTORY_SIZE = 50,    // Tamanho do historico.
		SPEED_BUFFER_SIZE = 10    // Tamanho do buffer usado para calcular a velocidade
	};

public:
	KNetStatsView(KNetStats *parent, const QString &interface);

	~KNetStatsView() { delete mStatistics; }

	void updateViewOptions();

	// read a value from /sys/class/net/interface/name
	unsigned long long readInterfaceNumValue(const char *name);

	const double *speedHistoryRx() const { return mSpeedHistoryRx; }

	const double *speedHistoryTx() const { return mSpeedHistoryTx; }

	const int historyBufferSize() const { return HISTORY_SIZE; }

	const int *historyPointer() const { return &mSpeedHistoryPtr; }

	const double *maxSpeed() const { return &mMaxSpeed; }

	///	The current monitored network interface
	inline const QNetworkInterface &interface() const;

	///	The current Update Interval in miliseconds
	inline int updateInterval() const;

	/// We are in textmode?
	inline ViewMode viewMode() const;

	inline unsigned long long totalBytesRx() const;

	inline unsigned long long totalBytesTx() const;

	inline unsigned long long totalPktRx() const;

	inline unsigned long long totalPktTx() const;

	inline double byteSpeedRx() const;

	inline double byteSpeedTx() const;

	inline double pktSpeedRx() const;

	inline double pktSpeedTx() const;

	const ViewOptions *getViewOptions() const { return &mOptions; }

protected:

//	void paintEvent(QPaintEvent *ev);

private:
	QString mSysDevPath;            // Path to the device.
	bool mCarrier;                    // Interface carrier is on?
	bool mConnected;                // Interface exists?

	QSystemTrayIcon *trayIcon;
	QMenu *mContextMenu;        // Global ContextMenu
	Statistics *mStatistics;        // Statistics window
	QNetworkInterface mInterface;                // Current interface
	ViewOptions mOptions;            // View options

	// Icons
	QIcon mIconError, mIconNone, mIconTx, mIconRx, mIconBoth;
	QIcon *mCurrentIcon{};            // Current state
	QTimer *mTimer;                    // Timer

	//	Rx e Tx to bytes and packets
	unsigned long long mBRx{}, mBTx{}, mPRx{}, mPTx{};
	// Statistics
	unsigned long long mTotalBytesRx{}, mTotalBytesTx{}, mTotalPktRx{}, mTotalPktTx{};
	// Speed buffers
	double mSpeedBufferRx[SPEED_BUFFER_SIZE]{}, mSpeedBufferTx[SPEED_BUFFER_SIZE]{};
	double mSpeedBufferPRx[SPEED_BUFFER_SIZE]{}, mSpeedBufferPTx[SPEED_BUFFER_SIZE]{};
	// pointer to current speed buffer position
	int mSpeedBufferPtr{};

	bool mFirstUpdate;

	// History buffer TODO: Make it configurable!
	double mSpeedHistoryRx[HISTORY_SIZE]{};
	double mSpeedHistoryTx[HISTORY_SIZE]{};
	int mSpeedHistoryPtr{};
	double mMaxSpeed{};
	int mMaxSpeedAge{};


	// set up the view.
	void setup();

	void drawText(QPainter &paint);

	void drawGraphic(QPainter &paint);

	// Reset speed and history buffers
	void resetBuffers();

	// calc tha max. speed stored in the history buffer
	inline void calcMaxSpeed();

	// calc the speed using a speed buffer
	inline double calcSpeed(const double *buffer) const;

private slots:

	// Called by the timer to update statistics
	void updateStats();

//	void showContextMenu(const QPoint &pos);
	void updateIcon();

	void iconActivated(QSystemTrayIcon::ActivationReason reason);
};

void KNetStatsView::calcMaxSpeed() {
	double max = 0.0;
	int ptr = mSpeedHistoryPtr;
	for (int i = 0; i < HISTORY_SIZE; ++i) {
		if (mSpeedHistoryRx[i] > max) {
			max = mSpeedHistoryRx[i];
			ptr = i;
		}
		if (mSpeedHistoryTx[i] > max) {
			max = mSpeedHistoryTx[i];
			ptr = i;
		}
	}
	mMaxSpeed = max;
	mMaxSpeedAge = (mSpeedHistoryPtr > ptr) ? (mSpeedHistoryPtr - ptr) : (mSpeedHistoryPtr + HISTORY_SIZE - ptr);
}

double KNetStatsView::calcSpeed(const double *buffer) const {
	double total = 0.0;
	for (int i = 0; i < SPEED_BUFFER_SIZE; ++i)
		total += buffer[i];
	return total / SPEED_BUFFER_SIZE;
}

const QNetworkInterface &KNetStatsView::interface() const {
	return mInterface;
}

int KNetStatsView::updateInterval() const {
	return mOptions.mUpdateInterval;
}

ViewMode KNetStatsView::viewMode() const {
	return mOptions.mViewMode;
}

unsigned long long KNetStatsView::totalBytesRx() const {
	return mTotalBytesRx;
}

unsigned long long KNetStatsView::totalBytesTx() const {
	return mTotalBytesTx;
}

unsigned long long KNetStatsView::totalPktRx() const {
	return mTotalPktRx;
}

unsigned long long KNetStatsView::totalPktTx() const {
	return mTotalPktTx;
}

double KNetStatsView::byteSpeedRx() const {
	return mSpeedHistoryRx[mSpeedHistoryPtr];
}

double KNetStatsView::byteSpeedTx() const {
	return mSpeedHistoryTx[mSpeedHistoryPtr];
}

double KNetStatsView::pktSpeedRx() const {
	return calcSpeed(mSpeedBufferPRx);
}

double KNetStatsView::pktSpeedTx() const {
	return calcSpeed(mSpeedBufferPTx);
}

#endif
