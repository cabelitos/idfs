#include "fs_message.hh"

QDataStream &operator<<(QDataStream &out, const FsMessage &fsMessage)
{
	out << fsMessage.host <<
		fsMessage.hostType << fsMessage.timeStamp;
	return out;
}

QDataStream &operator>>(QDataStream &in, FsMessage &fsMessage)
{
	in >> fsMessage.host >>
		fsMessage.hostType >> fsMessage.timeStamp;
	return in;
}

QDataStream &operator<<(QDataStream &out, const FsCommandMessage &fsCommandMessage)
{
	out << fsCommandMessage.commandType <<
		fsCommandMessage.args << fsCommandMessage.fileData;
	return out;
}

QDataStream &operator>>(QDataStream &in, FsCommandMessage &fsCommandMessage)
{
	in >> fsCommandMessage.commandType >>
		fsCommandMessage.args >> fsCommandMessage.fileData;
	return in;
}

QDataStream &operator<<(QDataStream &out, const FsReplyMessage &fsReplyMessage)
{
	out << fsReplyMessage.success << fsReplyMessage.message;
	return out;
}

QDataStream &operator>>(QDataStream &in, FsReplyMessage &fsReplyMessage)
{
	in >> fsReplyMessage.success >> fsReplyMessage.message;
	return in;
}

QDataStream &operator<<(QDataStream &out,
	const FsCommandMessage::FsCommandType &fsCommandType)
{
	out >> (quint32&)fsCommandType;
	return out;
}

QDataStream &operator>>(QDataStream &in,
	FsCommandMessage::FsCommandType &fsCommandType)
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