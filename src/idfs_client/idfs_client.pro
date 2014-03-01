TEMPLATE = app
QT += core network
LIBS += ../idfs_libs/libidfs_libs.a
PRE_TARGETDEPS += ../idfs_libs/libidfs_libs.a
INCLUDEPATH += ../idfs_libs
HEADERS += $$PWD/client_socket.hh
SOURCES += $$PWD/client_socket.cpp $$PWD/main.cpp

TARGET = idfs_client