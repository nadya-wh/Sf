#-------------------------------------------------
#
# Project created by QtCreator 2016-07-06T20:40:55
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = PartModel
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Sf.cpp \
    Com_Lib/Objects_sys.cpp \
    Com_Lib/Archive.cpp \
    Com_Lib/Objects.cpp \
    Sf_elem.cpp

HEADERS += \
    sf.h \
    Com_Lib/objects_sys.h \
    Com_Lib/archive.h \
    Com_Lib/objects.h
