#pragma once

#include <QTcpSocket>
#include <QHash>
#include <QTimer>
#include "fs_message.hh"
#include "idfs_socket.hh"

class SlaveNode : public IdfsSocket {
	Q_OBJECT

private:
	QHash <QString, QString> _fileLocations;
	QTimer _saveTimeout;
	bool _fileLocationsDirty;

	void _fileLocationsDiskOp(bool read = false);

private slots:
	void ready();
	void masterDown();
	void processMessage(FsMessage msg);
	void _timeout();

public:
	SlaveNode(QObject *parent = 0);
	virtual ~SlaveNode();
};