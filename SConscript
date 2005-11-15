#! /usr/bin/env python

Import( 'env' )


env.KDElang('po', 'knetstats')
env.docfolder('doc', 'en', 'knetstats')
env.KDEicon()
env.KDEinstall('KDEMENU', 'Internet', 'knetstats.desktop')

#if env['_INSTALL']:
import glob
env.KDEinstall('KDEDATA', 'knetstats/pics', glob.glob("src/pics/*.png"))
