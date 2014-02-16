#pragma once

#include <QMultiHash>
#include <QString>
#include "file_info.hh"

class FsTree : public QMultiHash <QString, FileInfo> {

private:
	bool _dirty; /* hmmm, bad boy ! */

public:

	FsTree();
	virtual ~FsTree();

	bool insert(const QString &path, const FileInfo &fileInfo);
	QString getParent(const QString &path);
	static FsTree loadFromDisk(const QString &path);
	bool sendToFile(const QString &path);
	int remove(const QString &path, const FileInfo &fileInfo);
	int remove(const QString &path);

	static const QString DIR_SEPARATOR;

	bool isDirty();

};