#include <QApplication>
#include <QResource>

#include "knetstats.h"

const char *programName = "KNetStats";

int main(int argc, char **argv) {
	QResource::registerResource("knetstats.rcc");
	QApplication::setOrganizationName("KNetStats");
	QApplication::setApplicationName("KNetStats");
	QApplication::setDesktopFileName("com.telans.KNetStats");
	QApplication::setQuitOnLastWindowClosed(false);
	QApplication app(argc, argv);
	KNetStats knetstats;

	return QApplication::exec();
}
