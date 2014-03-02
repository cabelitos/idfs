#pragma once

#include <QTcpSocket>
#include <QObject>
#include <QtGlobal>
#include "fs_message.hh"

class Q_DECL_EXPORT IdfsSocket : public QTcpSocket {
	Q_OBJECT

private slots:
	void _canRead();

public:
	IdfsSocket(QObject *parent = 0);
	virtual ~IdfsSocket();

	void sendFsMessage(const FsMessage &msg);

signals:
	void newMessage(FsMessage msg);
};