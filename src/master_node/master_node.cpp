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

	path = QStandardPaths::locate(QStandardPaths::DataLocation, FS_TREE_FILE_NAME);
	this->_files = FsTree::loadFromDisk(path);

	/* No previous file, create root ! */
	if (this->_files.empty())
	{
		FileInfo f;
		f.fileName = FsTree::DIR_SEPARATOR;
		f.isDir = true;
		f.cTime = QDateTime::currentDateTime();
		this->_files.insert(FsTree::DIR_SEPARATOR, f);
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

void MasterNode::incomingConnection(qintptr socketDescriptor)
{
	MasterNodeClient *client = new MasterNodeClient(this);
	if (!client->setSocketDescriptor(socketDescriptor))
	{
		qDebug() << "Could not set the descriptor:" <<
			socketDescriptor << " to the socket";
		delete client;
		return;
	}
	else
		qDebug() << "New client!";

	connect(client, SIGNAL(disconnected()), this, SLOT(_clientDisconnected()));
	connect(client, SIGNAL(newMessage(FsMessage)), this, SLOT(_clientMessage(FsMessage)));
	this->_clients.append(client);
}

void MasterNode::_clientDisconnected()
{
	MasterNodeClient *client;
	QMutableListIterator<MasterNodeClient *> it(this->_clients);

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

void MasterNode::_clientMessage(FsMessage fsMessage)
{
	Q_UNUSED(fsMessage);
	qDebug() << "New message!";
}

