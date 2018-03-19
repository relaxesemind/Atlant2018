#-------------------------------------------------
#
# Project created by QtCreator 2018-02-18T12:32:39
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14
#QMAKE_CXXFLAGS += -std = c++14

TARGET = ATLANT
TEMPLATE = app

VERSION = 1.0.0.1
QMAKE_TARGET_PRODUCT = ATLANT_mi
QMAKE_TARGET_COPYRIGHT = Egor Ivanov
QMAKE_TARGET_DESCRIPTION = UIR_2018

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\OpenCV\install\include

LIBS += -LC:\OpenCV\install\x86\mingw\lib
LIBS += -lopencv_core341 -lopencv_imgproc341 -lopencv_highgui341 \
-lopencv_ml341 -lopencv_video341 -lopencv_features2d341 -lopencv_calib3d341 \
-lopencv_objdetect341 -lopencv_flann341 -lopencv_videoio341



SOURCES += \
        main.cpp \
        mainwindow.cpp \
    cameracontroller.cpp \
    core.cpp \
    portcontroller.cpp

HEADERS += \
        mainwindow.h \
    cameracontroller.h \
    core.h \
    portcontroller.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resource\res.qrc


