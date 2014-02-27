#include "fs_message.hh"
#include <QDebug>

qint64 fsMessageSizeGet(const FsMessage &fsMessage)
{
	qint64 size = 0;

	size += ((sizeof(ushort) * fsMessage.host.length()) + sizeof(quint32));
	size += ((sizeof(ushort) * fsMessage.errorMessage.length()) + sizeof(quint32));
	size += (sizeof(quint32) * 3);
	size += sizeof(qint8); //bool
	size += (fsMessage.fileData.size() + sizeof(quint32));
	size += (sizeof(quint32) * 2);
	foreach (QString s, fsMessage.args)
		size += ((sizeof(ushort) * s.length()) + sizeof(quint32));
	size += ((sizeof(quint32) * 2) + sizeof(qint8)); //QDateTime
	return size;
}

QDataStream &operator<<(QDataStream &out, const FsMessage &fsMessage)
{
	out << fsMessage.host <<
		fsMessage.hostType << fsMessage.timeStamp << fsMessage.messageType
		<< fsMessage.commandType << fsMessage.args << fsMessage.fileData <<
		fsMessage.success << fsMessage.errorMessage;
	return out;
}

QDataStream &operator>>(QDataStream &in, FsMessage &fsMessage)
{
	in >> fsMessage.host >>
		fsMessage.hostType >> fsMessage.timeStamp >> fsMessage.messageType
		>> fsMessage.commandType >> fsMessage.args >> fsMessage.fileData >>
		fsMessage.success >> fsMessage.errorMessage;
	return in;
}

QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsCommandType &fsCommandType)
{
 	out << (quint32&)fsCommandType;
	return out;
}

QDataStream &operator>>(QDataStream &in,
	FsMessage::FsCommandType &fsCommandType)
{
	in >> (quint32&)fsCommandType;
	return in;
}

QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageHostType &fsMessageHostType)
{
	out << (quint32&)fsMessageHostType;
	return out;
}

QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageHostType &fsMessageHostType)
{
	in >> (quint32&)fsMessageHostType;
	return in;
}

QDataStream &operator<<(QDataStream &out,
	const FsMessage::FsMessageType &messageType)
{
	out << (quint32&)messageType;
	return out;
}

QDataStream &operator>>(QDataStream &in,
	FsMessage::FsMessageType &messageType)
{
	in >> (quint32&)messageType;
	return in;
}
