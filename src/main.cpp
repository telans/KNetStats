#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapp.h>
#include <klocale.h>

#include "knetstats.h"
const char* programName = I18N_NOOP( "KNetStats" );

int main( int argc, char** argv )
{
	KAboutData aboutData( "knetstats", programName,
	                      "v1.3",									// version
						  I18N_NOOP( "A network device monitor." ),	// description
						  KAboutData::License_GPL,					// license
	                      "(C) 2004-2005 Hugo Parente Lima",		// copyright
						  0,
						  "http://knetstats.sourceforge.net",		// homepage
						  "hugo_pl@users.sourceforge.net");			// bug email address
	aboutData.setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS"), I18N_NOOP("_: EMAIL OF TRANSLATORS"));
	aboutData.addAuthor( "Hugo Parente Lima", 0, "hugo_pl@users.sourceforge.net" );

	// Credits
	aboutData.addCredit( "Thomas Windheuser", I18N_NOOP("SConstruct help and Debian packages"), 0);
	aboutData.addCredit( "KNemo", I18N_NOOP("Icon themes"), 0,
						"http://kde-apps.org/content/show.php?content=12956" );
	// translators
	aboutData.addCredit( "Ilyas Bakirov", I18N_NOOP("Kirghiz translation"), 0);
	aboutData.addCredit( "Roberto Leandrini", I18N_NOOP("Italian translation"), 0);
	aboutData.addCredit( "Carlos Ortiz", I18N_NOOP("Spanish translation"), 0);
	aboutData.addCredit( "Henrik Gebauer", I18N_NOOP("Deutsch translation"), 0);
	aboutData.addCredit( "Edward Romantsov", I18N_NOOP("Russian translation"), 0);
	aboutData.addCredit( "Wiktor Wandachowicz", I18N_NOOP("Polish translation"), 0);

	KCmdLineArgs::init( argc, argv, &aboutData );

	KApplication::disableAutoDcopRegistration();
	KApplication app;
	KNetStats knetstats;
	if (!knetstats.allOk())
		return 1;
	return app.exec();
}

