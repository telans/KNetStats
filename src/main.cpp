#include <QApplication>
#include <QResource>

#include "knetstats.h"

const char *programName = "KNetStats";

int main(int argc, char **argv) {
	QResource::registerResource("knetstats.rcc");
	QApplication::setOrganizationName("KNetStats");
	QApplication::setApplicationName("KNetStats");
	QApplication::setWindowIcon(QIcon(":/images/knetstats-32.png"));
	QApplication::setQuitOnLastWindowClosed(false);
	QApplication app(argc, argv);
	KNetStats knetstats;

	if (!knetstats.canStart())
		return 1;
	return QApplication::exec();
}

