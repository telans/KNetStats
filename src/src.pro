# Arquivo gerado pelo gerenciador qmake do kdevelop.
# -------------------------------------------
# Subdiret�io relativo ao diret�io principal do projeto: ./src
# Alvo �um aplicativo:  ../bin/knetstats

INSTALLS += images \
            docs \
            desktop \
            target
KDEPREFIX = $$system(kde-config --prefix)

target.path = /usr/bin
target.extra = msgfmt -o /usr/share/locale/pt_BR/LC_MESSAGES po/pt_BR.po

desktop.files += ../knetstats.desktop
desktop.path = /usr/share/applications/
docs.files += ../doc/*
docs.path = /usr/share/doc/HTML/en/knetstats/
images.files += pics/*.png
images.path = /usr/share/apps/knetstats/pics/
FORMS += configurebase.ui \
         statisticsbase.ui
HEADERS += knetstats.h \
           configure.h \
           statistics.h
SOURCES += knetstats.cpp \
           main.cpp \
           configure.cpp \
           statistics.cpp
LIBS += -ldl \
-lkdecore \
-lkdeui
INCLUDEPATH = $$KDEPREFIX/include/kde $$KDEPREFIX/include
MOC_DIR = .moc
UI_DIR = .ui
OBJECTS_DIR = .o
QMAKE_LIBDIR = $$system(kde-config --expandvars --install lib)
TARGET = ../bin/knetstats
CONFIG += release \
warn_on \
qt
TEMPLATE = app
LANGUAGE = C++

