load(common_pre)

QT += core dbus testlib

TARGET = testinput
CONFIG   -= app_bundle
CONFIG   += blackconfig
CONFIG   += blackmisc
CONFIG   += testcase
CONFIG   += no_testcase_installs

TEMPLATE = app

DEPENDPATH += \
    . \
    $$SourceRoot/src \
    $$SourceRoot/tests \

INCLUDEPATH += \
    $$SourceRoot/src \
    $$SourceRoot/tests \

SOURCES += testinput.cpp

DESTDIR = $$DestRoot/bin

load(common_post)
