TEMPLATE = app
QT += core network

LIBS += ../idfs_libs/libidfs_libs.a

PRE_TARGETDEPS += ../idfs_libs/libidfs_libs.a

INCLUDEPATH += ../idfs_libs

HEADERS += $$PWD/slave_node.hh
SOURCES += $$PWD/slave_node.cpp $$PWD/main.cpp
TARGET = slave_node