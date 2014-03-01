TEMPLATE = lib
QT += core network
CONFIG += staticlib
HEADERS += $$PWD/idfs_socket.hh $$PWD/fs_message.hh
SOURCES += $$PWD/idfs_socket.cpp $$PWD/fs_message.cpp
TARGET = idfs_libs
