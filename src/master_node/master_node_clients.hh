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
	int _partCount;
	int _totalParts;
	QList<FsMessage> _filePartsMsg;

	void _sendFilePart();

public:
	MasterNodeClient(QObject *parent = 0);
	virtual ~MasterNodeClient();
	bool isSlave();
	const QString &getName() const;

	/* use this function when a lot of messeges will be sent in sequence.
		This function will manage to do not overflow the client's buffer.
	*/
	void pushFileMsg(const FsMessage &fsMessage);

signals:
	void masterNodeMsg(FsMessage fsMessage);
	void filePartSent(double percent, QString partName);

private slots:
	void _timeoutWrite();
	void _socketIdfsNewMessage(FsMessage msg);
};