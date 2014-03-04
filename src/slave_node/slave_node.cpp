#include "slave_node.hh"
#include <QDebug>
#include <QCoreApplication>
#include <QDataStream>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#define FILE_PARTS_FILE "file_parts.bin"
#define TIMEOUT_5_SEC (5000)

SlaveNode::SlaveNode(QObject *parent) : IdfsSocket(parent), _fileLocationsDirty(false)
{
	connect(this, SIGNAL(connected()), this, SLOT(_ready()));
	connect(this, SIGNAL(disconnected()), this, SLOT(_masterDown()));
	connect(this, SIGNAL(newMessage(FsMessage)), this,
		SLOT(_processMessage(FsMessage)));
	connect(&this->_saveTimeout, SIGNAL(timeout()), this, SLOT(_timeout()));

	QString aux = QStandardPaths::writableLocation(
		QStandardPaths::DataLocation);
	qDebug() << aux;

	QDir dir(aux);

	if (!dir.exists(aux) && !dir.mkdir(aux))
		qDebug() << "Could not create dir:" << aux;

	this->_saveTimeout.start(TIMEOUT_5_SEC);
	this->_fileLocationsDiskOp(true);
}

SlaveNode::~SlaveNode()
{
	this->disconnectFromHost();
	this->_fileLocationsDiskOp();
}

void SlaveNode::_fileLocationsDiskOp(bool read)
{
	QString path = QStandardPaths::writableLocation(
			QStandardPaths::DataLocation);

	path = path + QDir::separator() + FILE_PARTS_FILE;
	QFile file(path);
	QDataStream stream;
	stream.setDevice(&file);
	OpenMode om;

	if (!file.exists() && read)
		return;

	if (read)
		om = QIODevice::ReadOnly;
	else if (!read && !this->_fileLocationsDirty)
		return;
	else
		om = QIODevice::Truncate | QIODevice::WriteOnly;

	qDebug() << path;
	if (!file.open(om))
	{
		qDebug() << "Could not open the file:" << path;
		return;
	}

	if (read)
		stream >> this->_fileLocations;
	else
		stream << this->_fileLocations;

	file.close();
	this->_fileLocationsDirty = false;
}

void SlaveNode::_timeout()
{
	this->_fileLocationsDiskOp();
}

void SlaveNode::_processMessage(FsMessage msg)
{
	if (msg.messageType == FsMessage::FETCH_FILE)
	{
		FsMessage resp;
		resp.hostType = FsMessage::SLAVE_NODE;
		resp.messageType =  FsMessage::FILE_PART;
		QString fileName = this->_fileLocations.value(msg.args[0]);
		QFile file(fileName);
		file.open(QIODevice::ReadOnly);
		resp.fileData = file.readAll();
		file.close();
		resp.args << msg.args[0].split("-p")[0];
		resp.args << fileName.section("-p", -1);
		this->sendFsMessage(resp);
	}
	else if (msg.messageType == FsMessage::REPLY)
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
			QString fileFinalPath = dir.absoluteFilePath(msg.args[1]);
			QFile file(fileFinalPath);
			resp.success = file.open(QIODevice::Truncate |
				QIODevice::WriteOnly);
			if (!resp.success)
			{
				resp.errorMessage = "Could not create the file";
				goto send;
			}

			wrote = file.write(msg.fileData);
			if (wrote != msg.fileData.size())
			{
				resp.errorMessage = "Could not write the file";
				resp.success = false;
			}
			file.close();
			if (resp.success)
			{
				this->_fileLocations.insert(msg.args[1], fileFinalPath);
				this->_fileLocationsDirty = true;
			}
		}
send:
		resp.hostType = FsMessage::SLAVE_NODE;
		resp.messageType =  FsMessage::REPLY;
		this->sendFsMessage(resp);
	}

}

void SlaveNode::_ready()
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
	msg.messageType =  FsMessage::INTRODUCTION_SLAVE;
	this->sendFsMessage(msg);
}

void SlaveNode::_masterDown()
{
	qDebug() << "Disconnected from master node!";
	QCoreApplication::exit();
}