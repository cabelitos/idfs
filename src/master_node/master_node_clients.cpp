#include "master_node_clients.hh"
#include <QDataStream>
#include <QTimer>

MasterNodeClient::MasterNodeClient(QObject *parent) :
	QTcpSocket (parent), _isSlave(false), _timerStarted(false)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(canRead()));
}

MasterNodeClient::~MasterNodeClient()
{
}

void MasterNodeClient::sendFsMessage(const FsMessage &fsMessage)
{
	QDataStream stream(this);
	stream << fsMessageSizeGet(fsMessage) << fsMessage;
	this->flush();
}

bool MasterNodeClient::isSlave()
{
	return this->_isSlave;
}

void MasterNodeClient::_sendFilePart()
{
	if (!this->_filePartsMsg.isEmpty())
		this->sendFsMessage(this->_filePartsMsg.takeFirst());
}

void MasterNodeClient::timeoutWrite()
{
	this->_timerStarted = false;
	this->_sendFilePart();
}

void MasterNodeClient::pushFilePartMsg(const FsMessage &fsMessage)
{
	if (!this->_timerStarted)
	{
		this->_timerStarted = true;
		/* Start when you can. */
		QTimer::singleShot(1, this, SLOT(timeoutWrite()));
	}
	this->_filePartsMsg.append(fsMessage);
}

const QString &MasterNodeClient::getName() const
{
	return this->_name;
}

void MasterNodeClient::canRead()
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

	if (msg.messageType == FsMessage::REPLY)
	{
		this->_sendFilePart();
	}
	else if (msg.messageType == FsMessage::INTRODUCTION)
	{
		this->_isSlave = true;
		this->_name = msg.host;
		FsMessage response;
		response.messageType =	FsMessage::REPLY;
		response.hostType =	 FsMessage::MASTER_NODE;
		response.timeStamp = QDateTime::currentDateTime();
		response.success = true;
		this->sendFsMessage(response);
	}
	else
		emit this->newMessage(msg);
}

