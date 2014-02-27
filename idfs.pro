TEMPLATE = subdirs
SUBDIRS = src/idfs_libs src/master_node src/idfs_client src/slave_node
src/master_node.depends = src/idfs_libs
src/idfs_client.depends = src/idfs_libs
src/slave_node.depends = src/idfs_libs