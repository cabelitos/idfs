#include "client_socket.hh"
#include <QDebug>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>

ClientSocket::ClientSocket(QObject *parent) : IdfsSocket(parent)
{
	connect(this, SIGNAL(newMessage(FsMessage)), this,
		SLOT(processMessage(FsMessage)));
	connect(this, SIGNAL(connected()), this, SLOT(ready()));
	connect(this, SIGNAL(disconnected()), this, SLOT(goodbye()));
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::sendCommand(QStringList args)
{
	this->_commandArgs = args;
	if (this->state() == QAbstractSocket::ConnectedState)
		this->_createMessageAndSend();
}

void ClientSocket::_createMessageAndSend()
{
	if (this->_commandArgs.isEmpty())
	{
		qDebug() << "Nothing to send!";
		return;
	}

	FsMessage msg;
	msg.messageType = FsMessage::COMMAND;
	msg.hostType = FsMessage::CLIENT_APP;
	msg.timeStamp = QDateTime::currentDateTime();
	this->_command = this->_commandArgs.takeFirst().toLower();

	if (this->_command == "ls")
		msg.commandType = FsMessage::LS;
	else if (this->_command == "mkdir")
		msg.commandType = FsMessage::MKDIR;
	else if (this->_command == "touch")
		msg.commandType = FsMessage::TOUCH;
	else if (this->_command == "push_file")
	{
		QString path = this->_commandArgs.takeFirst();
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Can not open the file:" << path;
			this->disconnectFromHost();
			return;
		}
		msg.fileData = file.readAll();
		msg.commandType = FsMessage::PUSH_FILE;
		QFileInfo info(file);
		msg.args << info.fileName();
		file.close();
	}
	else
		msg.commandType = FsMessage::UNKNOWN_COMMAND;

	msg.args << this->_commandArgs;
	this->sendFsMessage(msg);
}

void ClientSocket::processMessage(FsMessage msg)
{
	if (msg.messageType != FsMessage::REPLY)
	{
		qDebug() << "Message Type not correct %d" << (int)msg.messageType;
		return;
	}
	else if (msg.hostType != FsMessage::MASTER_NODE)
	{
		qDebug() << "We are not supposed to receive messages from %d" << (int)msg.hostType;
		return;
	}

	if (!msg.success)
		qDebug() << "Error:" << msg.errorMessage;
	else
	{
		qDebug() << "Success";
		if (this->_command == "ls")
			qDebug() << msg.args;
	}

	this->disconnectFromHost();
}

void ClientSocket::ready()
{
	qDebug() << "Sending command";
	this->_createMessageAndSend();
}

void ClientSocket::goodbye()
{
	qDebug() << "Disconnected from master node!";
	QCoreApplication::exit();
}