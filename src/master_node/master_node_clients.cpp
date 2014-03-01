#include "master_node_clients.hh"
#include <QDataStream>
#include <QTimer>

MasterNodeClient::MasterNodeClient(QObject *parent) :
	IdfsSocket (parent), _isSlave(false), _timerStarted(false)
{
	connect(this, SIGNAL(newMessage(FsMessage)), this,
		SLOT(socketIdfsNewMessage(FsMessage)));
}

MasterNodeClient::~MasterNodeClient()
{
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

void MasterNodeClient::socketIdfsNewMessage(FsMessage msg)
{
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
		emit this->masterNodeMsg(msg);
}

