#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QDateTime>
#include <QList>
#include "fs_message.hh"
#include "idfs_socket.hh"


class MasterNodeClient : public IdfsSocket {
	Q_OBJECT

private:
	QString _name;
	bool _isSlave;
	bool _timerStarted;
	QList<FsMessage> _filePartsMsg;

	void _sendFilePart();

public:
	MasterNodeClient(QObject *parent = 0);
	virtual ~MasterNodeClient();
	bool isSlave();
	const QString &getName() const;

	void pushFilePartMsg(const FsMessage &fsMessage);

signals:
	void masterNodeMsg(FsMessage fsMessage);

private slots:
	void timeoutWrite();
	void socketIdfsNewMessage(FsMessage msg);
};