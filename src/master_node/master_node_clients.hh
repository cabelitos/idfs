#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QDateTime>
#include "fs_message.hh"


class MasterNodeClient : public QTcpSocket {
	Q_OBJECT

protected:
	QString _name;

	FsMessage readFsMessage();

public:
	MasterNodeClient(QObject *parent = 0);
	virtual ~MasterNodeClient();

	void sendFsMessage(const FsMessage &fsMessage);

signals:
	void newMessage(FsMessage fsMessage);

protected slots:
	virtual void canRead();
};