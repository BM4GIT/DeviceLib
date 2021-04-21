#-------------------------------------------------
#
# Project created by QtCreator 2021-02-07T14:07:52
#
#-------------------------------------------------

QT += core gui
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlowIt
TEMPLATE = app
DESTDIR = ../FlowIt Release/


SOURCES += main.cpp\
        flowit.cpp \
    column.cpp \
    flowobject.cpp \
    flowcheck.cpp \
    flowsensor.cpp \
    flowactuator.cpp \
    flowrepeat.cpp \
    devicedlg.cpp \
    objectwid.cpp \
    flowroutine.cpp \
    flowdo.cpp \
    filloutdlg.cpp \
    template.cpp \
    flowwait.cpp \
    flowwhile.cpp \
    askdlg.cpp \
    language.cpp \
    exportdlg.cpp \
    flowpage.cpp

HEADERS  += flowit.h \
    column.h \
    flowobject.h \
    flowcheck.h \
    flowsensor.h \
    flowactuator.h \
    flowrepeat.h \
    devicedlg.h \
    objectwid.h \
    flowroutine.h \
    flowdo.h \
    filloutdlg.h \
    template.h \
    flowwait.h \
    flowwhile.h \
    askdlg.h \
    language.h \
    exportdlg.h \
    flowpage.h

FORMS    += flowit.ui \
    devicedlg.ui \
    objectwid.ui \
    filloutdlg.ui \
    askdlg.ui \
    exportdlg.ui
RESOURCES   += FlowIt.qrc

RC_ICONS += FlowIt.ico

OTHER_FILES += \
    language.fi
