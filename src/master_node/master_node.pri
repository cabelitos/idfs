TEMPLATE = app
QT += core network

LIBS += $$OUT_PWD/bin/libidfs_libs.a

PRE_TARGETDEPS += $$OUT_PWD/bin/libidfs_libs.a

HEADERS += $$PWD/master_node.hh $$PWD/fs_tree.hh $$PWD/file_info.hh \
	$$PWD/master_node_clients.hh
SOURCES += $$PWD/master_node_clients.cpp $$PWD/file_info.cpp $$PWD/fs_tree.cpp \
	$$PWD/master_node.cpp $$PWD/main.cpp
TARGET = master_node