#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapp.h>
#include <klocale.h>
#include "knetstats.h"

static const char *appName = "knetstats";
const char *programName = I18N_NOOP( "KNetStats" );
static const char *version = "v1.0";
static const char *description = I18N_NOOP( "A network device monitor." );
static int licenseType = KAboutData::License_GPL;
static const char *copyRightStatement = "(C) 2004 Hugo Parente Lima";

int main( int argc, char** argv )
{
	KAboutData aboutData( appName, programName,
	                      version, description, licenseType,
	                      copyRightStatement );
	aboutData.addAuthor( "Hugo Parente Lima", 0, "hugo_pl@users.sourceforge.net" );

	KCmdLineArgs::init( argc, argv, &aboutData );

	KApplication app;
	KNetStats knetstats;
	knetstats.show();
	return app.exec();
}

