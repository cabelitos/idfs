#include "fs_tree.hh"
#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <QMutableListIterator>

const QString FsTree::DIR_SEPARATOR = "/";

FsTree::FsTree() : QMultiHash <QString, FileInfo> (), _dirty(false)
{
}

FsTree::~FsTree()
{
}

bool FsTree::isDirty()
{
	return this->_dirty;
}

int FsTree::remove(const QString &path, const FileInfo &fileInfo)
{
	int r;

	r = QMultiHash<QString, FileInfo>::remove(path, fileInfo);
	if (r > 0)
		this->_dirty = true;
	return r;
}

int FsTree::remove(const QString &path)
{
	int r;

	r = QMultiHash<QString, FileInfo>::remove(path);
	if (r > 0)
		this->_dirty = true;
	return r;
}

FsTree FsTree::loadFromDisk(const QString &path)
{
	QDataStream stream;
	QFile file(path);
	FsTree newTree;

	if (!file.exists())
		return newTree;

	if (!file.open(QIODevice::ReadOnly))
		return newTree;

	stream.setDevice(&file);
	stream >> newTree;
	qDebug() << "File system image at:" << path;
	return newTree;
}

bool FsTree::sendToFile(const QString &path)
{
	QFile file(path);
	QDataStream stream;

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	stream.setDevice(&file);
	stream << (*this);
	this->_dirty = false;
	return true;
}

bool FsTree::_fileExistInDir(const QString &path, const QString &name,
	FileInfo *outInfo)
{
	foreach (FileInfo info, this->values(path))
	{
		if (info.fileName == name)
		{
			if (outInfo)
				*outInfo = info;
			return true;
		}
	}
	return false;
}

QString FsTree::createPath(const QString &fileName, const QString &basePath)
{
	if (basePath == FsTree::DIR_SEPARATOR)
		return QString(basePath + fileName);
	return QString(basePath + FsTree::DIR_SEPARATOR + fileName);
}

bool FsTree::insert(const QString &path, const FileInfo &fileInfo)
{
	FileInfo outInfo;

	if (this->_fileExistInDir(path, fileInfo.fileName, &outInfo))
	{
		if (!outInfo.isDir)
		{
			qDebug() << path << " is not a dir!";
			return false;
		}
		else
			return true;
	}
	this->_dirty = true;
	QMultiHash<QString, FileInfo>::insert(path, fileInfo);
	return true;
}

bool FsTree::ls(const QString &path, QStringList &files)
{
	QString aux;

	if (path != FsTree::DIR_SEPARATOR && path.endsWith(FsTree::DIR_SEPARATOR))
		aux = path.left(path.size() - 1);
	else
		aux = path;
	foreach(FileInfo info, this->values(aux))
	{
		if (FsTree::DIR_SEPARATOR == info.fileName)
			continue;
		files << info.fileName;
	}
	return true;
}

bool FsTree::insertFile(const QString &path, qint64 fileSize,
	QString &errorMsg)
{
	QStringList splited = path.split(FsTree::DIR_SEPARATOR,
		QString::SkipEmptyParts);

	if (splited.size() == 0)
	{
		errorMsg = "Invalid path!";
		return false;
	}

	QString touchedFile = splited.takeLast();
	QString last = FsTree::DIR_SEPARATOR;
	foreach(QString s, splited)
	{
		FileInfo info;
		if (this->_fileExistInDir(last, s, &info))
		{
			if (!info.isDir)
			{
				errorMsg = s + " is not a dir";
				return false;
			}
		}
		else
		{
			errorMsg = "dir:" + s + " not found!";
			return false;
		}
		if (last.size() == 1)
			last += s;
		else
			last += FsTree::DIR_SEPARATOR + s;
	}

	if (this->_fileExistInDir(last, touchedFile, NULL))
	{
		errorMsg = "File already exists!";
		return false;
	}
	FileInfo newFile;
	newFile.fileName = touchedFile;
	newFile.isDir = false;
	newFile.cTime = QDateTime::currentDateTime();
	newFile.size = fileSize;
	this->insert(last, newFile);
	if (last != FsTree::DIR_SEPARATOR)
		last += FsTree::DIR_SEPARATOR + touchedFile;
	else
		last += touchedFile;
	/* Include itself, so it can be found if the user types ls /path/to/touch/file/touchedfile*/
	this->insert(last, newFile);
	return true;
}

bool FsTree::addChunckToFileInfo(const QString &path,
	const QString &slaveName, const QString &filePart)
{
	QList<FileInfo> infos = this->values(path);

	if (infos.isEmpty() || infos.size() > 1)
		return false;

	/* Oh god this is ugly as hell*/
	FileInfo newInfo = infos[0];
	newInfo.chunksLocation.append(qMakePair(slaveName, filePart));
	this->remove(path);
	this->insert(path, newInfo);
	return true;
}

bool FsTree::mkdir(const QString &path, QString &errorMsg)
{
	QStringList splited = path.split(FsTree::DIR_SEPARATOR,
		QString::SkipEmptyParts);
	bool r;
	if (splited.size() == 0)
	{
		r = false;
		errorMsg = "No root";
	}
	else
	{
		QString last = FsTree::DIR_SEPARATOR;
		r = true;
		foreach (QString s, splited)
		{
			FileInfo info;

			info.fileName = s;
			info.isDir = true;
			info.cTime = QDateTime::currentDateTime();
			if (!this->insert(last, info))
			{
				r = false;
				errorMsg = s + " is not a dir!";
				break;
			}

			if (last.size() == 1)
				last += s;
			else
				last += FsTree::DIR_SEPARATOR + s;
			qDebug() << last;
		}
	}
	foreach(QString s, this->keys())
	{
		qDebug() << "key-" << s;
		foreach(FileInfo i, this->values(s))
		{
			qDebug() << i.fileName;
		}
	}
	return r;
}
