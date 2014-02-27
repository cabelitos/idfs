#pragma once

#include <QTcpSocket>
#include "fs_message.hh"

class SlaveNode : public QTcpSocket {
	Q_OBJECT

private slots:
	void ready();
	void masterDown();
	void canRead();

private:
	void _sendMessage(const FsMessage &fsMessage);

public:
	SlaveNode(QObject *parent = 0);
	virtual ~SlaveNode();
};