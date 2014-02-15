#include "master_node.hh"
#include <QDebug>
#include <QTcpSocket>
#include <QMutableListIterator>
#include <QAbstractSocket>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

#define FS_TREE_FILE_NAME "fs_tree.bin"
#define TIMEOUT_5_MIN (300000)

MasterNode::MasterNode(QObject *parent) : QTcpServer(parent)
{
	QString path;
	connect(this, SIGNAL(newConnection()), this, SLOT(newConn()));

	path = QStandardPaths::locate(QStandardPaths::DataLocation, FS_TREE_FILE_NAME);
	this->_files = FsTree<FileInfo>::loadFromDisk(path);

	/* No previous file, create root ! */
	if (this->_files.empty())
	{
		FileInfo f;
		f.setFileName(FsTree<FileInfo>::DIR_SEPARATOR);
		f.setIsDir(true);
		f.setCTime(QDateTime::currentDateTime());
		this->_files.insert(FsTree<FileInfo>::DIR_SEPARATOR, f, true);
	}
	connect(&this->_saveTimeout, SIGNAL(timeout()), this, SLOT(_saveTree()));
	this->_saveTimeout.start(TIMEOUT_5_MIN);
}

MasterNode::~MasterNode()
{
	this->_saveTree();
}

void MasterNode::_saveTree()
{
	QString path("%1%2%3"), aux;
	QDir dir;

	if (!this->_files.isDirty())
		return;

	aux = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

	if (!dir.exists(aux) && !dir.mkdir(aux))
		qDebug() << "Could not create dir:" << aux;

	path = path.arg(aux).arg(QDir::separator()).arg(FS_TREE_FILE_NAME);

	if (!this->_files.sendToFile(path))
		qDebug() << "Could not save the fs tree at:" << path;
}

void MasterNode::newConn()
{
	QTcpSocket *client = this->nextPendingConnection();

	connect(client, SIGNAL(readyRead()), this, SLOT(clientCanRead()));
	connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
	qDebug() << "New client" << client;
	this->_clients.append(client);
}

void MasterNode::clientDisconnected()
{
	QTcpSocket *client;
	QMutableListIterator<QTcpSocket *> it(this->_clients);

	qDebug() << "Client disconnected !";
	while (it.hasNext())
	{
		client = it.next();
		if (client->state() == QAbstractSocket::UnconnectedState)
		{
			it.remove();
			client->deleteLater();
		}
	}
}

void MasterNode::clientCanRead()
{
	QTcpSocket *client;

	foreach(client, this->_clients)
	{
		if (client->bytesAvailable() < 1)
			continue;
	//TODO
		QDataStream stream(client);
		qDebug() << "3";
	}
}
