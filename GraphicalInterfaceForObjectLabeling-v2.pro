#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T01:51:44
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GraphicalInterfaceForObjectLabeling-v2
TEMPLATE = app


SOURCES += \
    src/form/mainwindow.cpp \
    src/main.cpp \
    src/ResultManager/ResultManager.cpp \
    src/ROI/CirROI.cpp \
    src/ROI/EllipROI.cpp \
    src/ROI/RectROI.cpp \
    src/ROI/ROIBase.cpp \
    src/ROIManager/ROIManager.cpp \
    src/Viewer/viewer.cpp

HEADERS += \
    src/common.h \
    src/form/mainwindow.h \
    src/ResultManager/ResultManager.h \
    src/ROI/CirROI.h \
    src/ROI/EllipROI.h \
    src/ROI/RectROI.h \
    src/ROI/ROIBase.h \
    src/ROIManager/ROIManager.h \
    src/Viewer/viewer.h

FORMS    += \
    src/form/mainwindow.ui

RESOURCES += \
    src/form/icon.qrc

INCLUDEPATH += \
    ./src \
    C:/opencv/build/include

LIBS += -LC:/opencv/build/x64/vc14/lib

    CONFIG(debug, release|debug) {
        LIBS += -lopencv_world310d
    } else {
        LIBS += -lopencv_world310
    }
