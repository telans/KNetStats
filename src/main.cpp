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
static const char *version = "v1.3";
static const char *description = I18N_NOOP( "A network device monitor." );
static int licenseType = KAboutData::License_GPL;
static const char *copyRightStatement = "(C) 2004 Hugo Parente Lima";

int main( int argc, char** argv )
{
	KAboutData aboutData( appName, programName,
	                      version, description, licenseType,
	                      copyRightStatement );
	aboutData.setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS"), I18N_NOOP("_: EMAIL OF TRANSLATORS"));
	aboutData.addAuthor( "Hugo Parente Lima", 0, "hugo_pl@users.sourceforge.net" );

	KCmdLineArgs::init( argc, argv, &aboutData );

	KApplication::disableAutoDcopRegistration();
	KApplication app;
	KNetStats knetstats;
	return app.exec();
}

