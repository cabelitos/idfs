#pragma once

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QList>
#include <QByteArray>

/* Since these are only messages that will be around in the network,
	leaving everything as public. Creating setters and getters will be a pain and it's
	not necessary.
*/

struct Q_CORE_EXPORT FsMessage {
	enum FsMessageHostType {
		UNKNOWN = 0,
		MASTER_NODE,
		SLAVE_NODE,
		CLIENT_APP
	};

	QString host;
	FsMessageHostType hostType;
	QDateTime timeStamp;
};

struct Q_CORE_EXPORT FsCommandMessage : public FsMessage {
	enum FsCommandType {
		UNKNOWN = 0,
		/* TODO - think about which commands we will support */
	};
	FsCommandType commandType;
	QList<QString> args;
	QByteArray fileData;
};

struct Q_CORE_EXPORT FsReplyMessage : public FsMessage {
	bool success;
	QString message; /* In case of error, we can specify something here. */
};

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out, const FsMessage &fsMessage);
QDataStream &operator>>(QDataStream &in, FsMessage &fsMessage);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out, const FsCommandMessage &fsCommandMessage);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in, FsCommandMessage &fsCommandMessage);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out, const FsReplyMessage &fsReplyMessage);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in, FsReplyMessage &fsReplyMessage);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsCommandMessage::FsCommandType &fsCommandType);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in,
	FsCommandMessage::FsCommandType &fsCommandType);

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageHostType &fsMessageHostType);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageHostType &fsMessageHostType);
