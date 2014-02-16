TEMPLATE = app
QT += core network
HEADERS += $$PWD/master_node.hh $$PWD/fs_tree.hh $$PWD/file_info.hh
SOURCES += $$PWD/file_info.cpp $$PWD/master_node.cpp $$PWD/main.cpp
TARGET = master_node