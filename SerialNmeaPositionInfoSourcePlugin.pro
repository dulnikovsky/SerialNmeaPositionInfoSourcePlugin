#-------------------------------------------------
#
# Project created by QtCreator 2019-05-29T18:36:31
#
#-------------------------------------------------

QT = core positioning serialport

CONFIG += qt plugin
QT += qml

TARGET = SerialNmeaPositionInfoSourcePlugin

TEMPLATE = lib

SOURCES += \
        serialnmeapositioninfosourceplugin.cpp \
    extendednmeapositioninfosource.cpp

HEADERS += \
        serialnmeapositioninfosourceplugin.h \
    extendednmeapositioninfosource.h

OTHER_FILES += \
    plugin.json

