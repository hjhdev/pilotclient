load(common_pre)

QT       += core dbus network multimedia

TARGET = sampleblackmiscquantities
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src/blackmisc
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)
