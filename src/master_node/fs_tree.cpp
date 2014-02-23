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

bool FsTree::_fileExistInDir(const QString &path, const QString &name)
{
	foreach (FileInfo info, this->values(path))
	{
		if (info.fileName == name)
			return true;
	}
	return false;
}

bool FsTree::insert(const QString &path, const FileInfo &fileInfo)
{
	if (this->_fileExistInDir(path, fileInfo.fileName))
	{
		qDebug() << " Already present!";
		return false;
	}
	this->_dirty = true;
	QMultiHash<QString, FileInfo>::insert(path, fileInfo);
	return true;
}

bool FsTree::ls(const QString &path, QStringList &files)
{
	foreach(FileInfo info, this->values(path))
	{
		if (FsTree::DIR_SEPARATOR == info.fileName)
			continue;
		files << info.fileName;
	}
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
		foreach (QString s, splited)
		{
			FileInfo info;

			info.fileName = s;
			info.isDir = true;
			info.cTime = QDateTime::currentDateTime();
			this->insert(last, info);

			if (last.size() == 1)
				last += s;
			else
				last += FsTree::DIR_SEPARATOR + s;
			qDebug() << last;
		}
		r = true;
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
