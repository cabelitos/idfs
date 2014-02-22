#include "master_node_clients.hh"
#include <QDataStream>

MasterNodeClient::MasterNodeClient(QObject *parent) :
	QTcpSocket (parent)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(canRead()));
}

MasterNodeClient::~MasterNodeClient()
{
}

FsMessage MasterNodeClient::readFsMessage()
{
	FsMessage fsMessage;
	QDataStream stream(this);
	stream >> fsMessage;
	return fsMessage;
}

void MasterNodeClient::sendFsMessage(const FsMessage &fsMessage)
{
	QDataStream stream(this);
	stream << fsMessage;
	this->flush();
}

void MasterNodeClient::canRead()
{
	emit this->newMessage(this->readFsMessage());
}

