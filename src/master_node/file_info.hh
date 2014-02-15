#pragma once

#include <QString>
#include <QList>
#include <QDateTime>
#include <QDataStream>
#include <QtGlobal>
#include <QPair>

class FileInfo {
private:
	QString _fileName;
	/* This pair specified which slave the chunk is
		located and the name of the chunk.
		One note, if fileInfo._isDir == true, this list will always be empty,
		because we won't save anything about dirs in the slave node. All we save is
		actual file data, not directories
	*/
	QList<QPair<QString, QString> > _chunksLocation;
	QDateTime _aTime; //access
	QDateTime _cTime; //creation
	QDateTime _mTime; //motification
	qint64 _size;
	bool _isDir;

public:
	FileInfo();
	virtual ~FileInfo();

	void setFileName(const QString &fileName);
	void setATime(const QDateTime &aTime);
	void setCTime(const QDateTime &cTime);
	void setMTime(const QDateTime &mTime);
	void setSize(qint64 size);
	void addChunkLocation(const QString &slaveName, const QString &chunkName);
	void setChunksLocation(const QList<QPair<QString, QString> > &chunksLocation);
	void setIsDir(bool isDir);

	qint64 getSize() const;
	bool isDir() const;
	const QDateTime &getATime() const;
	const QDateTime &getCTime() const;
	const QDateTime &getMTime() const;
	const QString &getFileName() const;
	const QList<QPair<QString, QString> > &getChunksLocation() const;
};

QDataStream &operator<<(QDataStream &out, const FileInfo &fileInfo);
QDataStream &operator>>(QDataStream &in, FileInfo &fileInfo);