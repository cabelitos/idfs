#pragma once

#include <QMultiHash>
#include <QString>
#include <QStringList>
#include "file_info.hh"

class FsTree : public QMultiHash <QString, FileInfo> {

private:
	bool _dirty; /* hmmm, bad boy ! */

	bool _fileExistInDir(const QString &path, const QString &name);
public:

	FsTree();
	virtual ~FsTree();

	bool insert(const QString &path, const FileInfo &fileInfo);
	QString getParent(const QString &path);
	static FsTree loadFromDisk(const QString &path);
	bool sendToFile(const QString &path);
	int remove(const QString &path, const FileInfo &fileInfo);
	int remove(const QString &path);

	bool mkdir(const QString &path, QString &errorMsg);
	bool ls(const QString &path, QStringList &files);

	static const QString DIR_SEPARATOR;

	bool isDirty();

};