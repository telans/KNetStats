# Arquivo gerado pelo gerenciador qmake do kdevelop. 
# ------------------------------------------- 
# Subdiretório relativo ao diretório principal do projeto: ./src
# Alvo é um aplicativo:  ../bin/knetstats

INSTALLS += images \
            docs \
            desktop \
            target 
target.path = /usr/bin 
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
INCLUDEPATH = $(KDEDIR)/include
MOC_DIR = .moc
UI_DIR = .ui
OBJECTS_DIR = .o
QMAKE_LIBDIR = $(KDEDIR)/lib
TARGET = ../bin/knetstats
CONFIG += release \
warn_on \
qt
TEMPLATE = app
LANGUAGE = C++

