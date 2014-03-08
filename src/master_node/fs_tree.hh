#pragma once

#include <QMultiHash>
#include <QString>
#include <QStringList>
#include "file_info.hh"

class FsTree : public QMultiHash <QString, FileInfo> {

private:
	bool _dirty; /* hmmm, bad boy ! */

	bool _fileExistInDir(const QString &path, const QString &name, FileInfo *outInfo);
public:

	FsTree();
	virtual ~FsTree();

	static FsTree loadFromDisk(const QString &path);
	bool sendToFile(const QString &path);

	/* Try not to use these */
	bool insert(const QString &path, const FileInfo &fileInfo);
	int remove(const QString &path, const FileInfo &fileInfo);
	int remove(const QString &path);
	/* End */

	bool mkdir(const QString &path, QString &errorMsg);
	bool ls(const QString &path, QStringList &files);
	bool insertFile(const QString &path, qint64 fileSize, QString &errorMsg);
	bool addChunckToFileInfo(const QString &path,const QString &slaveName, const QString &filePart);
	void rm(const QString &path);

	QString createPath(const QString &fileName, const QString &basePath);

	static const QString DIR_SEPARATOR;

	bool isDirty();

};