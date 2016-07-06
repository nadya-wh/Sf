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
    Sf_elem.cpp \
    Common/Bm.cpp \
    Common/Bv.cpp \
    Common/Sop.cpp \
    Common/Sop_Invers.cpp \
    Common/Sop_Equiv.cpp \
    Common/Sop_Elim.cpp \
    Common/Tv.cpp \
    Common/Tm_Invers.cpp \
    Common/Tm_Equiv.cpp \
    Common/Tm_Degen.cpp \
    Common/Tm.cpp \
    Com_Lib/Bufer.cpp

HEADERS += \
    sf.h \
    Com_Lib/objects_sys.h \
    Com_Lib/archive.h \
    Com_Lib/objects.h \
    Common/BaseBool.h \
    Common/BaseTern.h \
    Common/Sop.h \
    Com_Lib/bufer.h
