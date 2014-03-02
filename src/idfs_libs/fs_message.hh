#pragma once

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QList>
#include <QStringList>
#include <QByteArray>
#include <QtGlobal>

/* Data only, make as POD. */
/* TODO, This message is toooooooo complex, maybe we can remove some fields! */
struct Q_DECL_EXPORT FsMessage {
	enum FsMessageHostType {
		UNKNOWN_HOST = 0,
		MASTER_NODE,
		SLAVE_NODE,
		CLIENT_APP
	};

	enum FsCommandType {
		UNKNOWN_COMMAND = 0,
		LS,
		MKDIR,
		TOUCH,
		PUSH_FILE,
		GET_FILE
		/* TODO put commands here */
	};

	enum FsMessageType {
		UNKNOWN_MESSAGE = 0,
		COMMAND,
		INTRODUCTION,
		STORE_FILE,
		FILE_PART,
		FILE,
		REPLY,
		PROGRESS,
		FETCH_FILE
	};

	QString host;

	FsMessageHostType hostType;
	FsMessageType messageType;
	FsCommandType commandType;

	QDateTime timeStamp;
	QStringList args;
	QByteArray fileData;

	bool success;
	QString errorMessage; /* In case of error, we can specify something here. */
};

Q_DECL_EXPORT qint64 fsMessageSizeGet(const FsMessage &fsMessage);

Q_DECL_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage &fsMessage);
Q_DECL_EXPORT QDataStream &operator>>(QDataStream &in, FsMessage &fsMessage);

Q_DECL_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsCommandType &fsCommandType);
Q_DECL_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsCommandType &fsCommandType);

Q_DECL_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageHostType &fsMessageHostType);
Q_DECL_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageHostType &fsMessageHostType);

Q_DECL_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageType &messageType);
Q_DECL_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageType &messageType);
