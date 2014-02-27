#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QDateTime>
#include <QList>
#include "fs_message.hh"


class MasterNodeClient : public QTcpSocket {
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

	void sendFsMessage(const FsMessage &fsMessage);
	bool isSlave();
	const QString &getName() const;

	void pushFilePartMsg(const FsMessage &fsMessage);

signals:
	void newMessage(FsMessage fsMessage);

private slots:
	void canRead();
	void timeoutWrite();
};