#-------------------------------------------------
#
# Project created by QtCreator 2019-08-24T19:10:52
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OK64
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -O3
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        source/6502/impl.cpp \
        source/rvc.cpp \
        source/6502/mos6502.cpp \
        source/misc/util.cpp \
        source/rcomputer.cpp

HEADERS += \
        mainwindow.h \
        source/6502/impl.h \
        source/rvc.h \
        source/6502/mos6502.h \
        source/misc/util.h \
        source/rcomputer.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


    LIBS +=  -lresid


DISTFILES += \
    resources/text/opcodes.txt

RESOURCES += \
    resources.qrc
