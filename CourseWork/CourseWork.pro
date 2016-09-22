#-------------------------------------------------
#
# Project created by QtCreator 2016-09-17T18:43:50
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CourseWork
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../Com_Lib/Sflog.cpp \
    ../Com_Lib/Sf_Write.cpp \
    ../Com_Lib/Sf_SDF.cpp \
    ../Com_Lib/Sf_Read.cpp \
    ../Com_Lib/Sf_methods.cpp \
    ../Com_Lib/Sf_elem.cpp \
    ../Com_Lib/Sf_brace.cpp \
    ../Com_Lib/Sf.cpp \
    ../Com_Lib/Objects_sys.cpp \
    ../Com_Lib/Objects.cpp \
    ../Com_Lib/Dnf.cpp \
    ../Com_Lib/Bufer.cpp \
    ../Com_Lib/Brace.cpp \
    ../Com_Lib/Archive.cpp \
    ../ComBool/sTv.cpp \
    ../ComBool/sTm.cpp \
    ../ComBool/sBv.cpp \
    ../ComBool/sBm.cpp \
    ../Common/Tv.cpp \
    ../Common/Tm_Invers.cpp \
    ../Common/Tm_Equiv.cpp \
    ../Common/Tm_Degen.cpp \
    ../Common/Tm.cpp \
    ../Common/Sop_Invers.cpp \
    ../Common/Sop_Equiv.cpp \
    ../Common/Sop_Elim.cpp \
    ../Common/Sop.cpp \
    ../Common/Sbf.cpp \
    ../Common/Bv.cpp \
    ../Common/Bm.cpp

HEADERS += \
    ../Com_Lib/sflog.h \
    ../Com_Lib/sf.h \
    ../Com_Lib/objects_sys.h \
    ../Com_Lib/objects.h \
    ../Com_Lib/dnf.h \
    ../Com_Lib/bufer.h \
    ../Com_Lib/brace.h \
    ../Com_Lib/archive.h \
    ../ComBool/ShortTern.h \
    ../ComBool/ShortBool.h \
    ../Common/Sop.h \
    ../Common/Sbf.h \
    ../Common/BaseTern.h \
    ../Common/BaseBool.h
