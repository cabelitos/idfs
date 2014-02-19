TEMPLATE = subdirs
SUBDIRS = src/idfs_libs src/master_node src/idfs_client
src/master_node.depends = src/idfs_libs
src/idfs_client.depends = src/idfs_libs