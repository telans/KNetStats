
import os;
import re;
import commands;

def KDEPrefix( env, str ):
	if str == '':
		print "kde-config command not found!\nNote: Do not use \"su\" command to be a root user, because some problems can occur.";
		env.Exit(1);
	else:
		env['KDE_PREFIX'] = str.strip();

def KDELibs( env, str ):
	env['LIBPATH'] += [str.strip()];

#check if there is a different installation path as the default one
#(at the moment only used for building debian package)
customInstallPath = '';
if os.environ.has_key('DESTDIR') :
	customInstallPath = os.environ['DESTDIR'];
	print "Custom install path set to: "+customInstallPath+"\n";



env = Environment(
	tools = ['default','qt'],
	LIBS = ['dl', 'kdecore', 'kdeui'],
	CPPFLAGS = ['-O2', '-Wall', '-pipe', '-fomit-frame-pointer'],
	platform = 'posix'
);

# Avoid spreading .sconsign files everywhere
env.SConsignFile('scons_signatures')

#under debian and some other distributions, we have to set the plugindirectories, so that QT's uic will run.
#first we try to detect the distribution by using the "lsb_release" program, which is part of the linux standard base
(lsbReleaseStatus,lsbReleaseOutput) = commands.getstatusoutput("lsb_release -i -s");
if (lsbReleaseStatus==0) :
	osName = lsbReleaseOutput.strip();
	if re.compile("Debian").search(osName,0) :
		print osName + " detected";
		env['QT_UICDECLFLAGS'] += '-L /usr/lib/kde3/plugins/';
		env['QT_UICIMPLFLAGS'] += '-L /usr/lib/kde3/plugins/';
	elif re.compile("Mandrakelinux").search(osName,0) :
		print osName + " detected";
		env['QT_UICDECLFLAGS'] += '-L /usr/lib/kde3/plugins/';
		env['QT_UICIMPLFLAGS'] += '-L /usr/lib/kde3/plugins/';
	else :
		print "WARNING: The scons script could not recognice your kind of linux distribution.";
		print "         If you get a compile error, this could be the reason for it.";
		print "         In this case please file a bug report on https://sourceforge.net/projects/knetstats/.";
		print "         Please include the following information in the bug report:";
		print "         output of";
		print "         $>lsb_release -i -s";
		print "         and the path to the kde plugins for qt designer";
		print "         they are usually located in \"/usr/lib/qt3/plugins/designer\"";
		print "         just type on /";
		print "         $>find -name \"kdewidgets*\"";
else :
	print "WARNING: lsb_release not found. This could lead to an compile error.";
	print "         If you get a compile error, this could be the reason for it.";
	print "         Make sure you have lsb_release installed. It is part of the Linux Standard Base.";

env.ParseConfig('kde-config --prefix', KDEPrefix );
env.ParseConfig('kde-config --expandvars --install lib', KDELibs );
env['CPPPATH'] += [env['KDE_PREFIX']+'/include/', env['KDE_PREFIX']+'/include/kde' , './src' ];

# a custom builder to compile the translation files
poBuilder = Builder(action = 'msgfmt --output-file=$TARGET $SOURCE');
env['BUILDERS']['Po'] = poBuilder;

srcList = [ 'src/configurebase.ui', 'src/configure.cpp', 'src/statisticsbase.ui', 'src/main.cpp', 'src/knetstats.cpp', 'src/knetstatsview.cpp', 'src/statistics.cpp' ]
imageList = ['src/pics/theme0_none.png'
	,'src/pics/knetstats.png'
	,'src/pics/icon_pci.png'
	,'src/pics/theme0_rx.png'
	,'src/pics/theme0_tx.png'
	,'src/pics/theme0_both.png'
	,'src/pics/theme0_none.png'
	,'src/pics/theme0_error.png'
	,'src/pics/theme1_rx.png'
	,'src/pics/theme1_tx.png'
	,'src/pics/theme1_both.png'
	,'src/pics/theme1_none.png'
	,'src/pics/theme1_error.png'
	,'src/pics/theme2_rx.png'
	,'src/pics/theme2_tx.png'
	,'src/pics/theme2_both.png'
	,'src/pics/theme2_none.png'
	,'src/pics/theme2_error.png'
	,'src/pics/theme3_rx.png'
	,'src/pics/theme3_tx.png'
	,'src/pics/theme3_both.png'
	,'src/pics/theme3_none.png'
	,'src/pics/theme3_error.png'
	,'src/pics/theme4_rx.png'
	,'src/pics/theme4_tx.png'
	,'src/pics/theme4_both.png'
	,'src/pics/theme4_none.png'
	,'src/pics/theme4_error.png'];

env.Po('po/de.mo','po/de.po');
env.Po('po/es.mo','po/es.po');
env.Po('po/it.mo','po/it.po');
env.Po('po/ky_KG.mo','po/ky_KG.po');
env.Po('po/pl.mo','po/pl.po');
env.Po('po/pt_BR.mo','po/pt_BR.po');
env.Po('po/ru_RU.mo','po/ru_RU.po');
env.Po('po/fr.mo','po/fr.po');
env.Program('bin/knetstats', srcList)

# Installation
env.Alias( 'install', env.Install(customInstallPath+'/usr/bin', 'bin/knetstats') );
env.Alias( 'install', env.Install(customInstallPath+'/usr/share/applications/', 'knetstats.desktop') );
env.Alias( 'install', env.Install(customInstallPath+'/usr/share/doc/kde/HTML/en/knetstats/', 'doc/index.docbook') );
env.Alias( 'install', env.Install(customInstallPath+'/usr/share/apps/knetstats/pics/',imageList) );

# i18n installation
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/de/LC_MESSAGES/knetstats.mo', 'po/de.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/es/LC_MESSAGES/knetstats.mo', 'po/es.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/it/LC_MESSAGES/knetstats.mo', 'po/it.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/ky_KG/LC_MESSAGES/knetstats.mo', 'po/ky_KG.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/pl/LC_MESSAGES/knetstats.mo', 'po/pl.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/pt_BR/LC_MESSAGES/knetstats.mo', 'po/pt_BR.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/ru_RU/LC_MESSAGES/knetstats.mo', 'po/ru_RU.mo'));
env.Alias( 'install', env.InstallAs(customInstallPath+'/usr/share/locale/fr/LC_MESSAGES/knetstats.mo', 'po/fr.mo'));

env.Help("\nTo compile KNetStats type:\n"
	"  scons\n"
	"To install KNetStats type as root user:\n"
	"  scons install\n\n"
	"Of course you need compile KNetStats before installing it.\n"
	"If you want to install knetstats in a different directory, put the directory in the environment variable DESTDIR before calling \"scons install\""
	);
