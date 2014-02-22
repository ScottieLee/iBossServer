#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T18:44:04
#
#-------------------------------------------------

QT       += core
QT       += network
QT       += widgets
QT       += gui
QT += multimedia \
      multimediawidgets \
#QT       += multimedia

TARGET = iBossServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ibosstcpserver.cpp \
    filelist.cpp \
    musicfilelist.cpp

HEADERS += \
    ibosstcpserver.h \
    filelist.h \
    CodeDef.h \
    musicfilelist.h
