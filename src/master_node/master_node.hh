#pragma once

#include <QTcpServer>
#include <QObject>
#include <QList>
#include "fs_tree.hh"
#include "file_info.hh"

class MasterNode: public QTcpServer {
	Q_OBJECT

private:
	QList<QTcpSocket *> _clients;
	FsTree<FileInfo> _files;

public:
	MasterNode(QObject *parent = 0);
	virtual ~MasterNode();

private slots:
	void newConn();
	void clientDisconnected();
	void clientCanRead();
};
