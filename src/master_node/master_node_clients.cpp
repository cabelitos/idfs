#include "master_node_clients.hh"
#include <QDataStream>
#include <QTimer>

MasterNodeClient::MasterNodeClient(QObject *parent) :
	IdfsSocket (parent), _isSlave(false), _timerStarted(false),
	_partCount(0), _totalParts(0)
{
	connect(this, SIGNAL(newMessage(FsMessage)), this,
		SLOT(_socketIdfsNewMessage(FsMessage)));
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
	{
		FsMessage msg = this->_filePartsMsg.takeFirst();
		this->sendFsMessage(msg);
		if (msg.messageType == FsMessage::STORE_FILE)
		{
			double percent = ((++this->_partCount * 100.00) / this->_totalParts);
			emit this->filePartSent(percent, msg.args[1]);
		}
	}
	else
	{
		this->_partCount = 0;
		this->_totalParts = 0;
	}
}

void MasterNodeClient::_timeoutWrite()
{
	this->_timerStarted = false;
	this->_totalParts = this->_filePartsMsg.size();
	this->_sendFilePart();
}

void MasterNodeClient::pushFileMsg(const FsMessage &fsMessage)
{
	if (!this->_timerStarted)
	{
		this->_timerStarted = true;
		QTimer::singleShot(1000, this, SLOT(_timeoutWrite()));
	}
	this->_filePartsMsg.append(fsMessage);
}

const QString &MasterNodeClient::getName() const
{
	return this->_name;
}

void MasterNodeClient::_socketIdfsNewMessage(FsMessage msg)
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

