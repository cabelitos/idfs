#include "client_socket.hh"
#include <QDebug>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>

ClientSocket::ClientSocket(QObject *parent) : IdfsSocket(parent)
{
	connect(this, SIGNAL(newMessage(FsMessage)), this,
		SLOT(_processMessage(FsMessage)));
	connect(this, SIGNAL(connected()), this, SLOT(_ready()));
	connect(this, SIGNAL(disconnected()), this, SLOT(_goodbye()));
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
	else if (this->_command == "get_file" || this->_command == "cat")
		msg.commandType = FsMessage::GET_FILE;
	else if (this->_command == "rm")
		msg.commandType = FsMessage::RM;
	else
		msg.commandType = FsMessage::UNKNOWN_COMMAND;

	if (msg.commandType == FsMessage::GET_FILE)
		msg.args << this->_commandArgs[0];
	else
		msg.args << this->_commandArgs;
	this->sendFsMessage(msg);
}

void ClientSocket::_processMessage(FsMessage msg)
{
	if (msg.messageType != FsMessage::REPLY &&
		msg.messageType != FsMessage::PROGRESS &&
		msg.messageType != FsMessage::FILE)
	{
		qDebug() << "Message Type not correct:" << (int)msg.messageType;
		return;
	}
	else if (msg.hostType != FsMessage::MASTER_NODE)
	{
		qDebug() << "We are not supposed to receive messages from:" << (int)msg.hostType;
		return;
	}

	if (msg.messageType == FsMessage::REPLY)
	{
		if (!msg.success)
			qDebug() << "Error:" << msg.errorMessage;
		else
		{
			qDebug() << "Success";
			if (this->_command == "ls")
				qDebug() << msg.args;
		}

		if (!msg.success ||
			(this->_command != "push_file" && this->_command != "get_file"
			&& this->_command != "cat"))
			this->disconnectFromHost();
	}
	else if (msg.messageType ==  FsMessage::FILE)
	{
		if (this->_command == "get_file")
		{
			QString p = this->_commandArgs[1] + QDir::separator()+ msg.args[0];
			QFile file(p);
			file.open(QIODevice::WriteOnly);
			file.write(msg.fileData);
			file.close();
			qDebug() << "File saved at:" << p;
		}
		else /* cat */
			qDebug() << msg.fileData;
		this->disconnectFromHost();
	}
	else
	{
		qDebug() << "Percent:" << msg.args[0];
		FsMessage resp;
		resp.messageType = FsMessage::REPLY;
		resp.hostType = FsMessage::CLIENT_APP;
		resp.timeStamp = QDateTime::currentDateTime();
		resp.success = true;
		this->sendFsMessage(resp);
		if (msg.args[0].toDouble() == 100.00)
			this->disconnectFromHost();
	}
}

void ClientSocket::_ready()
{
	qDebug() << "Sending command";
	this->_createMessageAndSend();
}

void ClientSocket::_goodbye()
{
	qDebug() << "Disconnected from master node!";
	QCoreApplication::exit();
}