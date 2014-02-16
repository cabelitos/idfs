#include "fs_tree.hh"
#include <QDataStream>
#include <QFile>
#include <QDebug>

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

bool FsTree::insert(const QString &path, const FileInfo &fileInfo)
{
	if (this->contains(path) && fileInfo.isDir)
	{
		qDebug() << "Dir: " << path << " already present!";
		return false;
	}
	this->_dirty = true;
	QMultiHash<QString, FileInfo>::insert(path, fileInfo);
	return true;
}

QString FsTree::getParent(const QString &path)
{
	int last;

	if (!this->contains(path) || path == FsTree::DIR_SEPARATOR)
		return QString::Null();

	last = path.lastIndexOf(FsTree::DIR_SEPARATOR);

	if (last < 0)
		return QString::Null();

	if (last == 0) /* root !*/
		return QString(FsTree::DIR_SEPARATOR);

	QString r = path;
	r.replace(last, path.size() - last, "");
	return r;
}