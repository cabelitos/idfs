CONFIG += debug
OBJECTS_DIR = objs
MOC_DIR = mocs
DESTDIR = bin

LIBS += -Lbin/ -lidfs_libs
INCLUDEPATH += src/lib
DEPENDPATH += src/lib

macx {
PRE_TARGETDEPS += bin/libidfs_libs.dylib
}
#TODO - add win32 and unix:!macx

include(src/lib/idfs_libs.pri)
include(src/master_node/master_node.pri)