#include "slave_node.hh"
#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#define SLAVE_DIR "data"

SlaveNode::SlaveNode(QObject *parent) : QTcpSocket(parent)
{
	connect(this, SIGNAL(connected()), this, SLOT(ready()));
	connect(this, SIGNAL(disconnected()), this, SLOT(masterDown()));
	connect(this, SIGNAL(readyRead()), this, SLOT(canRead()));
	QString aux = QStandardPaths::writableLocation(
		QStandardPaths::DataLocation);
	qDebug() << aux;
	QDir dir(aux);

	if (!dir.exists(aux) && !dir.mkdir(aux))
		qDebug() << "Could not create dir:" << aux;
}

SlaveNode::~SlaveNode()
{
	this->disconnectFromHost();
}

void SlaveNode::_sendMessage(const FsMessage &fsMessage)
{
	QDataStream stream(this);
	stream << fsMessageSizeGet(fsMessage) << fsMessage;
	this->flush();
}

void SlaveNode::canRead()
{
	QDataStream stream(this);
	static qint64 fsMessageSize = 0;

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
		if (!msg.success)
			qDebug() << "Error:" << msg.errorMessage;
		else
			qDebug() << "No errors";
	}
	else if (msg.messageType == FsMessage::STORE_FILE)
	{
		FsMessage resp;
		qint64 wrote;

		QString dataPath = QStandardPaths::writableLocation(
			QStandardPaths::DataLocation);
		QString partFilePath = msg.args[0];
		QString after;
		if (partFilePath.size() == 1)
			after = "root";
		else
			after = QString("root") + QDir::separator();
		partFilePath = partFilePath.replace(0, 1, after);
		partFilePath = dataPath + QDir::separator() + partFilePath;
		QDir dir;

		resp.success = dir.mkpath(partFilePath);
		if (!resp.success)
		{
			resp.errorMessage = "Could not create the folder for the file";
			goto send;
		}
		{
			dir.cd(partFilePath);
			QFile file(dir.absoluteFilePath(msg.args[1]));
			resp.success = file.open(QIODevice::WriteOnly);
			if (!resp.success)
			{
				resp.errorMessage = "Could not create the file";
				goto send;
			}

			wrote = file.write(msg.fileData);
			if (wrote != msg.fileData.size())
				resp.errorMessage = "Could not write the file";
			file.close();
		}
send:
		resp.hostType = FsMessage::SLAVE_NODE;
		resp.messageType =  FsMessage::REPLY;
		this->_sendMessage(resp);
	}

}

void SlaveNode::ready()
{
	FsMessage msg;

	foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
	{
		if (address.protocol() == QAbstractSocket::IPv4Protocol &&
			address != QHostAddress(QHostAddress::LocalHost))
		{
			msg.host = address.toString();
			break;
		}
	}

	msg.hostType = FsMessage::SLAVE_NODE;
	msg.messageType =  FsMessage::INTRODUCTION;
	this->_sendMessage(msg);
}

void SlaveNode::masterDown()
{
	qDebug() << "Disconnected from master node!";
	QCoreApplication::exit();
}