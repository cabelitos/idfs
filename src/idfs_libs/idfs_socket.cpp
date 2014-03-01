#include "idfs_socket.hh"
#include <QDataStream>
#include <QDebug>

IdfsSocket::IdfsSocket(QObject *parent) : QTcpSocket(parent)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(_canRead()));
}

IdfsSocket::~IdfsSocket()
{
}

void IdfsSocket::sendFsMessage(const FsMessage &msg)
{
	QDataStream stream(this);
	stream << fsMessageSizeGet(msg) << msg;
	this->flush();
}

void IdfsSocket::_canRead()
{
	static qint64 fsMessageSize = 0;
	QDataStream stream(this);

	if (!fsMessageSize)
	{
		if (this->bytesAvailable() < (int)sizeof(fsMessageSize))
			return;
		stream >> fsMessageSize;
	}

	if (this->bytesAvailable() < fsMessageSize)
		return;

	FsMessage msg;
	stream >> msg;
	fsMessageSize = 0;
	emit this->newMessage(msg);
}