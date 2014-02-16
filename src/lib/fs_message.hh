#pragma once

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QList>
#include <QByteArray>

/* Data only, make as POD. */

struct Q_CORE_EXPORT FsMessage {
	enum FsMessageHostType {
		UNKNOWN_HOST = 0,
		MASTER_NODE,
		SLAVE_NODE,
		CLIENT_APP
	};

	enum FsCommandType {
		UNKNOWN_COMMAND = 0,
		/* TODO put commands here */
	};

	enum FsMessageType {
		UNKNOWN_MESSAGE = 0,
		COMMAND,
		REPLY
	};

	QString host;
	FsMessageHostType hostType;
	QDateTime timeStamp;

	FsMessageType messageType;
	FsCommandType commandType;
	QList<QString> args;
	QByteArray fileData;

	bool success;
	QString errorMessage; /* In case of error, we can specify something here. */
};

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage &fsMessage);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in, FsMessage &fsMessage);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsCommandType &fsCommandType);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsCommandType &fsCommandType);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageHostType &fsMessageHostType);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageHostType &fsMessageHostType);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageType &messageType);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageType &messageType);
