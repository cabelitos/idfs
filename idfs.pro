CONFIG += debug
OBJECTS_DIR = objs
MOC_DIR = mocs
DESTDIR = bin

INCLUDEPATH += src/lib
DEPENDPATH += src/lib

include(src/lib/idfs_libs.pri)
include(src/master_node/master_node.pri)