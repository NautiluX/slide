#-------------------------------------------------
#
# Project created by QtCreator 2019-01-20T10:45:40
#
#-------------------------------------------------

QT       += core gui
# CONFIG += qt debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = slide
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

mac: INCLUDEPATH += $$system(brew --prefix libexif)/include/
mac: QMAKE_LFLAGS += -L$$system(brew --prefix libexif)/lib



SOURCES += \
        main.cpp \
        mainwindow.cpp \
        imageswitcher.cpp \
        pathtraverser.cpp \
        overlay.cpp \
        imageselector.cpp \
        appconfig.cpp

HEADERS += \
        mainwindow.h \
        imageselector.h \
        pathtraverser.h \
        overlay.h \
        imageswitcher.h \
        imagestructs.h \
        appconfig.h

FORMS += \
        mainwindow.ui

target.path = /usr/local/bin/
INSTALLS += target

unix|win32: LIBS += -lexif
