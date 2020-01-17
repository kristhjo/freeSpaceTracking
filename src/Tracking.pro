#-------------------------------------------------
#
# Project created by QtCreator 2019-03-04T12:51:53
#
#-------------------------------------------------

QT       += core gui
QT       += charts
QT       += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl quick gui

TARGET = Tracking
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
        main.cpp \
        trackinggui.cpp \
    cameragui.cpp \
    imageprocessing.cpp \
    seeinggui.cpp \
    qcustomplot.cpp \
    hexapodgui.cpp \
    hedylamarrgui.cpp \
    plotdisplay.cpp

HEADERS += \
        trackinggui.h \
    cameragui.h \
    datacontainers.h \
    imageprocessing.h \
    seeinggui.h \
    qcustomplot.h \
    hexapodgui.h \
    hedylamarrgui.h \
    plotdisplay.h

FORMS += \
        trackinggui.ui \
    hexapodgui.ui \
    hedylamarrgui.ui \
    seeinggui.ui \
    plotdisplay.ui


INCLUDEPATH += $$PWD/inc/hexapod
DEPENDPATH += $$PWD/inc/hexapod

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/lib/ -lbgapi2_ext
unix:!macx: LIBS += -L$$PWD/lib/ -lbgapi2_ext_sc
unix:!macx: LIBS += -L$$PWD/lib/ -lbgapi2_genicam
unix:!macx: LIBS += -L$$PWD/lib/ -lGCBase_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -lGenApi_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -llog4cpp_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -lLog_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -lMathParser_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -lNodeMapData_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -lXmlParser_gcc41_v3_0_baumer
unix:!macx: LIBS += -L$$PWD/lib/ -lpi_pi_gcs2

INCLUDEPATH += $$PWD/inc
DEPENDPATH += $$PWD/inc

DISTFILES += \
    config_IQOQI.ini \
    config_BISAM.ini
