#include "master_node.hh"
#include "file_info.hh"
#include <QDebug>
#include <QTcpSocket>
#include <QMutableListIterator>
#include <QListIterator>
#include <QAbstractSocket>
#include <QDataStream>
#include <QStandardPaths>
#include <QDir>

#define FS_TREE_FILE_NAME "fs_tree.bin"
#define TIMEOUT_5_SEC (5000)
#define BLOCK_SIZE (512)

MasterNode::MasterNode(QObject *parent) : QTcpServer(parent)
{
	QString path;

	path = QStandardPaths::locate(QStandardPaths::DataLocation, FS_TREE_FILE_NAME);
	this->_files = FsTree::loadFromDisk(path);

	/* No previous file, create root ! */
	if (this->_files.empty())
	{
		FileInfo f;
		f.fileName = FsTree::DIR_SEPARATOR;
		f.isDir = true;
		f.cTime = QDateTime::currentDateTime();
		this->_files.insert(FsTree::DIR_SEPARATOR, f);
	}
	connect(&this->_saveTimeout, SIGNAL(timeout()), this, SLOT(_saveTree()));
	this->_saveTimeout.start(TIMEOUT_5_SEC);
}

MasterNode::~MasterNode()
{
	this->_saveTree();
}

void MasterNode::_saveTree()
{
	QString path("%1%2%3"), aux;
	QDir dir;

	if (!this->_files.isDirty())
		return;

	aux = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

	if (!dir.exists(aux) && !dir.mkdir(aux))
		qDebug() << "Could not create dir:" << aux;

	path = path.arg(aux).arg(QDir::separator()).arg(FS_TREE_FILE_NAME);

	if (!this->_files.sendToFile(path))
		qDebug() << "Could not save the fs tree at:" << path;
}

void MasterNode::incomingConnection(qintptr socketDescriptor)
{
	MasterNodeClient *client = new MasterNodeClient(this);
	if (!client->setSocketDescriptor(socketDescriptor))
	{
		qDebug() << "Could not set the descriptor:" <<
			socketDescriptor << " to the socket";
		delete client;
	}
	else
	{
		qDebug() << "New client!";
		connect(client, SIGNAL(disconnected()), this,
			SLOT(_clientDisconnected()));
		connect(client, SIGNAL(masterNodeMsg(FsMessage)), this,
			SLOT(_clientMessage(FsMessage)));
		this->_clients.append(client);
	}
}

void MasterNode::_clientDisconnected()
{
	MasterNodeClient *client;
	QMutableListIterator<MasterNodeClient *> it(this->_clients);

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

bool MasterNode::_sendFileToSlaves(const QString &path,
	const QString &fileName, const QString &fullPath,
	const QByteArray &fileData, QString &errorMsg)
{
	QList<MasterNodeClient *> slaves;

	foreach (MasterNodeClient *client, this->_clients)
	{
		if (client->isSlave())
			slaves.append(client);
	}

	if (slaves.isEmpty())
	{
		errorMsg = "There are no slave nodes connected!";
		return false;
	}

	QListIterator<MasterNodeClient *> it(slaves);
	qint64 pNum, pos;

	pos = pNum = 0;

	while (pos < fileData.size())
	{
		FsMessage msg;
		qint64 read;
		QString partName;
		MasterNodeClient *slave;

		if (pos + BLOCK_SIZE > fileData.size())
			read = fileData.size() - pos;
		else
			read = BLOCK_SIZE;

		msg.messageType =	FsMessage::STORE_FILE;
		msg.hostType =	 FsMessage::MASTER_NODE;
		msg.timeStamp = QDateTime::currentDateTime();

		msg.fileData = fileData.mid(pos, read);
		partName = fileName + "-p" + QString::number(pNum++);
		msg.args << path;
		msg.args << partName;
		slave = it.next();
		slave->pushFilePartMsg(msg);
		this->_files.addChunckToFileInfo(fullPath, slave->getName(), path+partName);
		if (!it.hasNext())
			it.toFront();
		pos += read;
	}
	return true;
}

void MasterNode::_clientMessage(FsMessage fsMessage)
{
	qDebug() << "New message!";
	FsMessage response;

	response.messageType =	FsMessage::REPLY;
	response.hostType =	 FsMessage::MASTER_NODE;
	response.timeStamp = QDateTime::currentDateTime();
	if (fsMessage.messageType != FsMessage::COMMAND)
	{
		response.success = false;
		response.errorMessage = "Message must be a command!";
	}
	else
	{
		if (fsMessage.commandType == FsMessage::UNKNOWN_COMMAND)
		{
			response.success = false;
			response.errorMessage = "Unknown command!";
		}
		else if (fsMessage.commandType == FsMessage::LS)
		{
			response.success = this->_files.ls(fsMessage.args[0], response.args);
		}
		else if (fsMessage.commandType == FsMessage::MKDIR)
		{
			response.success = this->_files.mkdir(fsMessage.args[0],
				response.errorMessage);
		}
		else if (fsMessage.commandType == FsMessage::TOUCH)
		{
			response.success = this->_files.insertFile(fsMessage.args[0],
				0, response.errorMessage);
		}
		else if (fsMessage.commandType == FsMessage::PUSH_FILE)
		{
			QString path =
				this->_files.createPath(fsMessage.args[0],fsMessage.args[1]);
			response.success = this->_files.insertFile(path,
				fsMessage.fileData.size(), response.errorMessage);
			if (response.success)
			{
				response.success = this->_sendFileToSlaves(fsMessage.args[1],
					fsMessage.args[0], path, fsMessage.fileData, response.errorMessage);
			}
		}
		else
		{
			response.success = false;
			response.errorMessage = "Command not implemented!";
		}
	}
	/* Evil, but necessary */
	MasterNodeClient *sender = dynamic_cast<MasterNodeClient*>(this->sender());
	sender->sendFsMessage(response);
}

