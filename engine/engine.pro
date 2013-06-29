#-------------------------------------------------
#
# Project created by QtCreator 2013-06-28T21:05:37
#
#-------------------------------------------------

QT       += core gui opengl quick

TARGET = engine
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += src/sceneview.cpp \
    src/main.cpp \
    src/engine/renderer.cpp

HEADERS  += src/sceneview.h \
    src/engine/renderer.h

OTHER_FILES += \
    Resources/main.qml \
    shaders/triangle.vert \
    shaders/triangle.frag \
    main.qml

RESOURCES += \
    main.qrc

LIBS += -lGLEW
