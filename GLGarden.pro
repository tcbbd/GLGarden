#-------------------------------------------------
#
# Project created by QtCreator 2014-11-26T21:42:21
#
#-------------------------------------------------

QT       += core gui opengl
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GLGarden
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gardenwidget.cpp \
    skybox.cpp \
    camera.cpp \
    terrain.cpp \
    tree.cpp \
    treedialog.cpp \
    snow.cpp \
    snowdialog.cpp \
    naivetree.cpp \
    naivetreedialog.cpp

HEADERS  += mainwindow.h \
    gardenwidget.h \
    skybox.h \
    camera.h \
    terrain.h \
    tree.h \
    treedialog.h \
    snow.h \
    snowdialog.h \
    naivetree.h \
    naivetreedialog.h

FORMS    += mainwindow.ui \
    treedialog.ui \
    snowdialog.ui \
    naivetreedialog.ui
