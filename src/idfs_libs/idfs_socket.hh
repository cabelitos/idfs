#pragma once

#include <QTcpSocket>
#include <QObject>
#include "fs_message.hh"

class Q_CORE_EXPORT IdfsSocket : public QTcpSocket {
	Q_OBJECT

private slots:
	void canRead();

public:
	IdfsSocket(QObject *parent = 0);
	virtual ~IdfsSocket();

	void sendFsMessage(const FsMessage &msg);

signals:
	void newMessage(FsMessage msg);
};