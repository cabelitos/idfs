#pragma once

#include <QTcpServer>
#include <QObject>
#include <QList>
#include "fs_tree.hh"
#include "file_info.hh"
#include "master_node_clients.hh"
#include "fs_message.hh"
#include <QTimer>

class MasterNode: public QTcpServer {
	Q_OBJECT

private:
	QList<MasterNodeClient *> _clients;
	FsTree _files;
	QTimer _saveTimeout;
	QHash<QString, MasterNodeClient*> _fileProgressReport;

	bool _sendFileToSlaves(const QString &path, const QString &filePath,
		const QString &fullPath,  MasterNodeClient *requester,
		const QByteArray &fileData, QString &errorMsg);

protected:
	void incomingConnection(qintptr socketDescriptor);

private slots:
	void _saveTree();
	void _clientDisconnected();
	void _clientMessage(FsMessage fsMessage);
	void _filePartSent(double percent, QString partName);

public:
	MasterNode(QObject *parent = 0);
	virtual ~MasterNode();
};
