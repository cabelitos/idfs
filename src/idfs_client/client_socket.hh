#pragma once

#include <QTcpSocket>
#include <QObject>
#include <QString>
#include <QStringList>
#include "idfs_socket.hh"

class ClientSocket : public IdfsSocket {

	Q_OBJECT

private:
	QStringList _commandArgs;
	QString _command;

	void _createMessageAndSend();

public:
	ClientSocket(QObject *parent = 0);
	virtual ~ClientSocket();

	void sendCommand(QStringList args);

private slots:
	void _processMessage(FsMessage msg);
	void _ready();
	void _goodbye();
};