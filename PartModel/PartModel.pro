#-------------------------------------------------
#
# Project created by QtCreator 2016-07-06T20:40:55
#
#-------------------------------------------------

QT       += core
#QT       -= gui
#QT       += list

TARGET = PartModel
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Sf.cpp \
    Com_Lib/Objects_sys.cpp \
    Com_Lib/Archive.cpp \
    Com_Lib/Objects.cpp \
    Common/Bm.cpp \
    Common/Bv.cpp \
    Common/Sop.cpp \
    Common/Sop_Invers.cpp \
    Common/Sop_Equiv.cpp \
    Common/Sop_Elim.cpp \
    Com_Lib/Bufer.cpp \
    Common/Tv.cpp \
    Common/Tm_Invers.cpp \
    Common/Tm_Equiv.cpp \
    Common/Tm_Degen.cpp \
    Common/Tm.cpp \
    ComBool/sTv.cpp \
    ComBool/sTm.cpp \
    ComBool/sBv.cpp \
    ComBool/sBm.cpp \
    Sf_elem.cpp \
    Common/Sbf.cpp \
    Sflog.cpp \
    Sf_Write.cpp \
    Sf_Read.cpp \
    Sf_SDF.cpp \
    Sf_brace.cpp \
    Com_Lib/Brace.cpp \
    Sf_methods.cpp \
    Com_Lib/Dnf.cpp

HEADERS += \
    sf.h \
    Com_Lib/objects_sys.h \
    Com_Lib/archive.h \
    Com_Lib/objects.h \
    Common/BaseBool.h \
    Common/BaseTern.h \
    Common/Sop.h \
    Com_Lib/bufer.h \
    ComBool/ShortTern.h \
    ComBool/ShortBool.h \
    Common/Sbf.h \
    sflog.h \
    Com_Lib/brace.h \
    Com_Lib/dnf.h
