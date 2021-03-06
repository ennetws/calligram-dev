QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = voronoi-tester
TEMPLATE = app
INCLUDEPATH += . ..

SOURCES  += main.cpp mainwindow.cpp
HEADERS  += mainwindow.h \
    nanoflann.hpp \
    kdtree.h

FORMS    += mainwindow.ui

# Libraries
CONFIG(debug, debug|release) {CFG = debug} else {CFG = release}
LIBS += -L$$PWD/../voronoi/$$CFG/lib -lvoronoi
INCLUDEPATH += ../voronoi
