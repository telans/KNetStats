#! /usr/bin/env python

"""
help       -> scons -h
compile    -> scons
clean      -> scons -c
install    -> scons install
uninstall  -> scons -c install
configure  -> scons configure prefix=/tmp/ita debug=full extraincludes=/usr/local/include:/tmp/include prefix=/usr/local

Run from a subdirectory -> scons -u
The variables are saved automatically after the first run (look at cache/kde.cache.py, ..)
"""

###################################################################
# LOAD THE ENVIRONMENT AND SET UP THE TOOLS
###################################################################

def bstr(target, source, env):
	comp='kconfig_compiler -d%s %s %s' % (str(source[0].get_dir()), source[1].path, source[0].path)
	return comp

## Load the builders in config
env = Environment( tools=['default', 'generic', 'kde'], toolpath=['./', './admin'])

env.KDEuse("environ rpath")
#env.KDEuse("environ rpath lang_qt thread nohelp")

env['CXXCOMSTR']=bstr

###################################################################
# SCRIPTS FOR BUILDING THE TARGETS
###################################################################

## BuilDir example - try to have all sources to process in only one top-level folder
#SetOption('duplicate', 'soft-copy')
#env.BuildDir('#build/kde3_examples', '#kde3_examples')
env.subdirs('src ./')

## Normal build
#env.subdirs('kde3_examples')

###################################################################
# CONVENIENCE FUNCTIONS TO EMULATE 'make dist' and 'make distclean'
###################################################################

## If your app name and version number are defined in 'version.h', use this instead:
## (contributed by Dennis Schridde devurandom@gmx@net)
#import re
#INFO = dict( re.findall( '(?m)^#define\s+(\w+)\s+(.*)(?<=\S)', open(r"version.h","rb").read() ) )
#APPNAME = INFO['KNetStats']
#VERSION = INFO['1.5']

# if you forget to add a version number, the one in the file VERSION will be used instead
env.dist('knetstats', '1.5')

#env.dist('knetstats')

