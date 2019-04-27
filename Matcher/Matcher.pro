#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T22:05:50
#
#-------------------------------------------------

QT       -= gui

TARGET = Matcher
TEMPLATE = lib

DEFINES += MATCHER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CFLAGS_ISYSTEM=
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        matcher.cpp

HEADERS += \
        matcher.h \
        matcher_global.h \ 
    matcher_config.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# OpenCV
unix:!macx: LIBS += -lopencv_imgproc
unix:!macx: LIBS += -lopencv_highgui
unix:!macx: LIBS += -lopencv_core
unix:!macx: LIBS += -lopencv_imgcodecs
unix:!macx: LIBS += -lopencv_xfeatures2d

INCLUDEPATH += /usr/include/opencv4/
DEPENDPATH += /usr/include/opencv4/
