#pragma once

#include <QTcpServer>
#include <QObject>
#include <QList>
#include "fs_tree.hh"
#include "file_info.hh"
#include "master_node_clients.hh"
#include "fs_message.hh"
#include <QTimer>
#include <QByteArray>
#include <QPair>

class MasterNode: public QTcpServer {
	Q_OBJECT

private:
	typedef struct _FetchFileInfo {
		MasterNodeClient *requester;
		FileInfo fileInfo;
		QList<QPair<int, QByteArray> > parts;
		qint64 total;
	} FetchFileInfo;
	QList<MasterNodeClient *> _clients;
	FsTree _files;
	QTimer _saveTimeout;
	QHash<QString, MasterNodeClient*> _fileProgressReport;
	QHash<QString, FetchFileInfo*> _fetchFileInfos;

	bool _sendFileToSlaves(const QString &path, const QString &filePath,
		const QString &fullPath,  MasterNodeClient *requester,
		const QByteArray &fileData, QString &errorMsg);

	bool _fetchFileParts(const FileInfo &info, QString &errorMsg);
	void sendFileToClient(FetchFileInfo *info);
	QList<MasterNodeClient*> _slavesGet();

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
