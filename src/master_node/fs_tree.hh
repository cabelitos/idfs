#pragma once

#include <QMultiHash>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDataStream>

template <class T>
class FsTree : public QMultiHash <QString, T> {

public:

	FsTree() : QMultiHash <QString, T> (){}
	virtual ~FsTree() {}

	bool insert(const QString &path, const T &t, bool isDir = false);
	QString getParent(const QString &path);
	static FsTree<T> loadFromDisk(const QString &path);
	bool sendToFile(const QString &path);

	static const QString DIR_SEPARATOR;

};

template <class T>
const QString FsTree<T>::DIR_SEPARATOR = "/";

template <class T>
FsTree<T> FsTree<T>::loadFromDisk(const QString &path)
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

template <class T>
bool FsTree<T>::sendToFile(const QString &path)
{
	QFile file(path);
	QDataStream stream;

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	stream.setDevice(&file);
	stream << (*this);
	return true;
}

template <class T>
bool FsTree<T>::insert(const QString &path, const T &t, bool isDir)
{
	if (this->contains(path) && isDir)
	{
		qDebug() << "Dir: " << path << " already present!";
		return false;
	}
	QMultiHash<QString, T>::insert(path, t);
	return true;
}

template <class T>
QString FsTree<T>::getParent(const QString &path)
{
	int last;

	if (!this->contains(path) || path == FsTree<T>::DIR_SEPARATOR)
		return QString::Null();

	last = path.lastIndexOf(FsTree<T>::DIR_SEPARATOR);

	if (last < 0)
		return QString::Null();

	if (last == 0) /* root !*/
		return QString(FsTree<T>::DIR_SEPARATOR);

	QString r = path;
	r.replace(last, path.size() - last, "");
	return r;
}