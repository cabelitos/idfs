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
#include <QtAlgorithms>

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
	qDeleteAll(this->_fetchFileInfos);
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
		connect(client, SIGNAL(slaveIntroduction()), this,
			SLOT(_addToSlaves()));
		connect(client, SIGNAL(masterNodeMsg(FsMessage)), this,
			SLOT(_clientMessage(FsMessage)));
		connect(client, SIGNAL(filePartSent(double, QString)), this,
			SLOT(_filePartSent(double, QString)));
	}
}

void MasterNode::_addToSlaves()
{
	MasterNodeClient *sender = dynamic_cast<MasterNodeClient*>(this->sender());
	connect(sender, SIGNAL(disconnected()), this, SLOT(_slaveDisconnected()));
	this->_slaves.append(sender);
}

void MasterNode::_filePartSent(double percent, QString partName)
{
	FsMessage response;

	response.messageType =	FsMessage::PROGRESS;
	response.hostType =	 FsMessage::MASTER_NODE;
	response.timeStamp = QDateTime::currentDateTime();
	response.success = true;
	response.args << QString::number(percent);
	if (this->_fileProgressReport.contains(partName))
		this->_fileProgressReport.take(partName)->pushFileMsg(response);
	else
		qDebug() << partName << " not present in _fileProgressReport";
}

void MasterNode::_slaveDisconnected()
{
	MasterNodeClient *client;
	QMutableListIterator<MasterNodeClient *> it(this->_slaves);

	qDebug() << "Client disconnected !";
	while (it.hasNext())
	{
		client = it.next();
		if (client->state() == QAbstractSocket::UnconnectedState)
			it.remove();
	}
}

bool MasterNode::_sendFileToSlaves(const QString &path,
	const QString &fileName, const QString &fullPath,
	MasterNodeClient *requester,
	const QByteArray &fileData, QString &errorMsg)
{
	if (this->_slaves.isEmpty())
	{
		errorMsg = "There are no slave nodes connected!";
		return false;
	}

	QListIterator<MasterNodeClient *> it(this->_slaves);
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
		slave->pushFileMsg(msg);
		this->_files.addChunckToFileInfo(fullPath, slave->getName(), partName);
		if (!it.hasNext())
			it.toFront();
		pos += read;
		this->_fileProgressReport.insert(partName, requester);
	}
	return true;
}

bool MasterNode::_fetchFileParts(const FileInfo &info, QString &errorMsg)
{
	if (this->_slaves.isEmpty())
	{
		errorMsg = "There are no slave nodes connected!";
		return false;
	}

	QPair<QString,QString> p;

	foreach (p, info.chunksLocation)
	{
		MasterNodeClient *found = 0;
		foreach(MasterNodeClient *slave, this->_slaves)
		{
			if (slave->getName() == p.first)
			{
				found = slave;
				break;
			}
		}

		if (!found)
		{
			errorMsg = "Slave:" + p.first + " not found!";
			foreach(MasterNodeClient *slave, this->_slaves)
				slave->popFileMsgs();
			return false;
		}

		FsMessage msg;
		msg.messageType =	FsMessage::FETCH_FILE;
		msg.hostType =	 FsMessage::MASTER_NODE;
		msg.timeStamp = QDateTime::currentDateTime();
		msg.args << p.second;
		found->pushFileMsg(msg);
	}

	return true;
}

static bool _filePartsSort(const QPair<int, QByteArray> &p1,
	const QPair<int, QByteArray> &p2)
{
	return p1.first < p2.first;
}

void MasterNode::_sendFileToClient(FetchFileInfo *info)
{
	QByteArray final;
	QPair<int,QByteArray> p;

	foreach (p, info->parts)
		final.append(p.second);

	FsMessage msg;
	msg.hostType =	 FsMessage::MASTER_NODE;
	msg.timeStamp = QDateTime::currentDateTime();
	msg.messageType =	FsMessage::FILE;
	msg.fileData = final;
	msg.args << info->fileInfo.fileName;
	info->requester->sendFsMessage(msg);
}

bool MasterNode::_fileDelete(const FileInfo &info, QString errorMsg)
{
	/* Before delete, check if all nodes that contains the file part are online */
	QPair<QString, QString> p;
	QList <QPair <QString, MasterNodeClient *> > toDelete;

	foreach (p, info.chunksLocation)
	{
		bool found = false;
		foreach (MasterNodeClient *slave, this->_slaves)
		{
			if (p.first == slave->getName())
			{
				found = true;
				toDelete.append(qMakePair(p.second, slave));
				break;
			}
		}
		if (!found)
		{
			errorMsg = "The slave node " + p.first+ " is not online!";
			return false;
		}
	}

	QPair<QString, MasterNodeClient *> deletePair;
	foreach (deletePair, toDelete)
	{
		FsMessage msg;
		msg.hostType =	 FsMessage::MASTER_NODE;
		msg.timeStamp = QDateTime::currentDateTime();
		msg.messageType =	FsMessage::DELETE_FILE;
		msg.args << deletePair.first;
		deletePair.second->pushFileMsg(msg);
	}

	return true;
}

void MasterNode::_clientMessage(FsMessage fsMessage)
{
	qDebug() << "New message!";
	FsMessage response;
	MasterNodeClient *sender = dynamic_cast<MasterNodeClient*>(this->sender());

	response.messageType =	FsMessage::REPLY;
	response.hostType =	 FsMessage::MASTER_NODE;
	response.timeStamp = QDateTime::currentDateTime();

	if (fsMessage.hostType == FsMessage::SLAVE_NODE &&
		fsMessage.messageType == FsMessage::FILE_PART)
	{
		FetchFileInfo *fetchInfo = this->_fetchFileInfos[fsMessage.args[0]];
		fetchInfo->parts << qMakePair(fsMessage.args[1].toInt(),
			fsMessage.fileData);
		fetchInfo->total += fsMessage.fileData.size();
		qDebug() << fetchInfo->total << "-" << fetchInfo->fileInfo.size;
		if (fetchInfo->total == fetchInfo->fileInfo.size)
		{
			qSort(fetchInfo->parts.begin(), fetchInfo->parts.end(),
				_filePartsSort);
			this->_sendFileToClient(fetchInfo);
			this->_fetchFileInfos.remove(fsMessage.args[0]);
			delete fetchInfo;
		}
	}
	else if (fsMessage.hostType == FsMessage::CLIENT_APP &&
			fsMessage.messageType != FsMessage::COMMAND)
	{
		response.success = false;
		response.errorMessage = "Message must be a command!";
	}
	else if (fsMessage.hostType == FsMessage::CLIENT_APP &&
			fsMessage.messageType == FsMessage::COMMAND)
	{
		if (fsMessage.commandType == FsMessage::UNKNOWN_COMMAND)
		{
			response.success = false;
			response.errorMessage = "Unknown command!";
		}
		else if(fsMessage.commandType == FsMessage::RM)
		{
			QList <FileInfo> infos = this->_files.values(fsMessage.args[0]);
			if (infos.isEmpty() || infos.size() > 1)
			{
				response.success = false;
				response.errorMessage = "You can only delete one file at once";
			}
			else if (this->_slaves.isEmpty())
			{
				response.success = false;
				response.errorMessage = "There are no slaves connected!";
			}
			else
			{
				response.success = _fileDelete(infos[0],
					response.errorMessage);
				if (response.success)
					this->_files.rm(fsMessage.args[0]);
			}
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
					fsMessage.args[0], path, sender, fsMessage.fileData,
					response.errorMessage);
				/* TODO: If failed remove from this->_files !*/
			}
		}
		else if (fsMessage.commandType == FsMessage::GET_FILE)
		{
			QList <FileInfo> infos = this->_files.values(fsMessage.args[0]);
			if (infos.isEmpty() || infos.size() > 1)
			{
				response.success = false;
				response.errorMessage = "No mathing file!";
			}
			else
			{
				response.success =
					this->_fetchFileParts(infos[0], response.errorMessage);

				if (response.success)
				{
					FetchFileInfo *fetchInfo = new FetchFileInfo;
					fetchInfo->fileInfo = infos[0];
					fetchInfo->requester = sender;
					fetchInfo->total = 0;
					this->_fetchFileInfos.insert(infos[0].fileName, fetchInfo);
					qDebug() << infos[0].fileName;
				}
			}
		}
		else
		{
			response.success = false;
			response.errorMessage = "Error!";
		}
		sender->sendFsMessage(response);
	}
}

