#ifndef KNETSTATSVIEW_H
#define KNETSTATSVIEW_H

#include <arpa/inet.h>
#include <linux/netdevice.h>
#include <QSystemTrayIcon>
#include <dirent.h>
#include "configure.h"
#include "knetstats.h"
#include "statistics.h"

class KNetStatsView : public QWidget {
Q_OBJECT

public:
	enum BUFFER_SIZES {
		HISTORY_SIZE = 50,    // Tamanho do historico.
		SPEED_BUFFER_SIZE = 10    // Tamanho do buffer usado para calcular a velocidade
	};

	//	Rx e Tx to bytes and packets
	unsigned long long mBRx{}, mBTx{}, mPRx{}, mPTx{};
	// Statistics
	unsigned long long mTotalBytesRx{}, mTotalBytesTx{}, mTotalPktRx{}, mTotalPktTx{};
	// Speed buffers
	double mSpeedBufferRx[SPEED_BUFFER_SIZE]{}, mSpeedBufferTx[SPEED_BUFFER_SIZE]{};
	double mSpeedBufferPRx[SPEED_BUFFER_SIZE]{}, mSpeedBufferPTx[SPEED_BUFFER_SIZE]{};
	// pointer to current speed buffer position
	int mSpeedBufferPtr{};
	int mSpeedHistoryPtr{};

	// History buffer TODO: Make it configurable!
	double mSpeedHistoryRx[HISTORY_SIZE]{};
	double mSpeedHistoryTx[HISTORY_SIZE]{};
	double mMaxSpeed{};
	int mMaxSpeedAge{};
	QString mInterface;                // Current interface

	KNetStatsView(KNetStats *parent, const QString &interface);

	~KNetStatsView() override { delete mStatistics; }

	void updateViewOptions();

	// read a value from /sys/class/net/interface/name
	unsigned long long readInterfaceNumValue(const char *name);

	///	The current Update Interval in miliseconds
	inline int updateInterval() const;

	const ViewOptions *getViewOptions() const { return &mOptions; }

	// calc the speed using a speed buffer
	static inline double calcSpeed(const double *buffer);

	inline bool interfaceIsValid() {
		bool ret = false;
		DIR *dir = opendir(mSysDevPath.toLatin1());
		if (dir)
			ret = true;
		closedir(dir);
		return ret;
	};

	inline bool trayIconVisible() { return mTrayIcon->isVisible(); }

private:
	KNetStats *mParent;
	QString mSysDevPath;            // Path to the device.
	bool mCarrier;                    // Interface carrier is on?
	QSystemTrayIcon *mTrayIcon;
	QMenu *mContextMenu;
	Statistics *mStatistics;        // Statistics window
	ViewOptions mOptions;            // View options
	// Icons
	QIcon mIconError, mIconNone, mIconTx, mIconRx, mIconBoth;
	QIcon *mCurrentIcon{};            // Current state
	QTimer *mTimer;                    // Timer
	bool mFirstUpdate;

	// set up the view.
	void setupTrayIcon();

	void updateStats();

	// calc tha max. speed stored in the history buffer
	inline void calcMaxSpeed();

private slots:

	void setupView();

	void checkMissingInterface();

	void iconActivated(QSystemTrayIcon::ActivationReason reason);

	void interfaceMissing();
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

double KNetStatsView::calcSpeed(const double *buffer) {
	double total = 0.0;
	for (int i = 0; i < SPEED_BUFFER_SIZE; ++i)
		total += buffer[i];
	return total / SPEED_BUFFER_SIZE;
}

int KNetStatsView::updateInterval() const {
	return mOptions.mUpdateInterval;
}

#endif
