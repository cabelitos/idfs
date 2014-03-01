#pragma once

#include <QTcpSocket>
#include "fs_message.hh"
#include "idfs_socket.hh"

class SlaveNode : public IdfsSocket {
	Q_OBJECT

private slots:
	void ready();
	void masterDown();
	void processMessage(FsMessage msg);

public:
	SlaveNode(QObject *parent = 0);
	virtual ~SlaveNode();
};