#pragma once

#include <QString>
#include <QList>
#include <QDateTime>
#include <QDataStream>
#include <QtGlobal>
#include <QPair>

struct FileInfo {
	QString fileName;
	/* This pair specified which slave the chunk is
		located and the name of the chunk.
		One note, if fileInfo._isDir == true, this list will always be empty,
		because we won't save anything about dirs in the slave node. All we save is
		actual file data, not directories
	*/
	QList<QPair<QString, QString> > chunksLocation;
	QDateTime aTime; //access
	QDateTime cTime; //creation
	QDateTime mTime; //motification
	qint64 size;
	bool isDir;

	bool operator==(const FileInfo &other);
};

QDataStream &operator<<(QDataStream &out, const FileInfo &fileInfo);
QDataStream &operator>>(QDataStream &in, FileInfo &fileInfo);