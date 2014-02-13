#include "master_node.hh"
#include <QDebug>
#include <QTcpSocket>
#include <QMutableListIterator>
#include <QAbstractSocket>
#include <QDataStream>
#include <QStandardPaths> //Use this to load the FsTree

MasterNode::MasterNode(QObject *parent) : QTcpServer(parent)
{
	connect(this, SIGNAL(newConnection()), this, SLOT(newConn()));
}

MasterNode::~MasterNode()
{
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
