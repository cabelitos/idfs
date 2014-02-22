TEMPLATE = app
QT += core network
LIBS += ../idfs_libs/libidfs_libs.a
PRE_TARGETDEPS += ../idfs_libs/libidfs_libs.a
INCLUDEPATH += ../idfs_libs
SOURCES += $$PWD/main.cpp

TARGET = idfs_client