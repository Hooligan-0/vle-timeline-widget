#-------------------------------
#
# Project created by QtCreator
#
#-------------------------------

QT       += core gui svg xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = svg-timeline
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    svgview.cpp \
    vlePlan.cpp \
    svgconfig.cpp

HEADERS  += mainwindow.h \
    svgview.h \
    vlePlan.h \
    svgconfig.h

FORMS    += mainwindow.ui

CONFIG   += console
