
import os;

def KDEPrefix( env, str ):
	if str == '':
		print "kde-config command not found!\nNote: Do not use \"su\" command to be a root user, because some problems can occur.";
		env.Exit(1);
	else:
		env['KDE_PREFIX'] = str.strip();

def KDELibs( env, str ):
	env['LIBPATH'] += [str.strip()];

env = Environment(
				tools = ['default','qt'],
				LIBS = ['dl', 'kdecore', 'kdeui'],
				plataform = 'posix'
			);
env.ParseConfig('kde-config --prefix', KDEPrefix );
env.ParseConfig('kde-config --expandvars --install lib', KDELibs );
env['CPPPATH'] += [env['KDE_PREFIX']+'/include/', env['KDE_PREFIX']+'/include/kde' ];

SrcList = [ 'src/configurebase.ui', 'src/statisticsbase.ui', 'src/main.cpp', 'src/knetstats.cpp', 'src/statistics.cpp', 'src/configure.cpp' ]

env.Program('bin/knetstats', SrcList)

# Installation
env.Alias( target='install', source=env.Install('/usr/bin', 'bin/knetstats') );
env.Alias( target='install', source=env.Install('/usr/share/applications/', 'knetstats.desktop') );
env.Alias( target='install', source=env.Install('/usr/doc/HTML/en/knetstats/', 'doc/index.docbook') );
env.Alias( target='install', source=env.Install('/usr/share/apps/knetstats/pics/', Split('src/pics/icon_both.png  src/pics/icon_configure.png  src/pics/icon_error.png  src/pics/icon_none.png  src/pics/icon_rx.png  src/pics/icon_tx.png  src/pics/knetstats.png')) );
# i18n installation
env.Alias( target='install', source=env.InstallAs('/usr/share/locale/pt_BR/LC_MESSAGES/knetstats.mo', 'po/pt_BR.mo') );
env.Alias( target='install', source=env.InstallAs('/usr/share/locale/ky_KG/LC_MESSAGES/knetstats.mo', 'po/ky_KG.mo') );

env.Help(	"\nTo compile KNetStats type:\n"
			"  scons\n"
			"To install KNetStats type as root user:\n"
			"  scons install\n\n"
			"Offcourse you need compile KNetStats before installing it."
		);
