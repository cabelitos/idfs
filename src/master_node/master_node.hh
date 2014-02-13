#pragma once

#include <QTcpServer>
#include <QObject>
#include <QList>

class MasterNode: public QTcpServer {
	Q_OBJECT

private:
	QList<QTcpSocket *> _clients;
public:
	MasterNode(QObject *parent = 0);
	virtual ~MasterNode();

private slots:
	void newConn();
	void clientDisconnected();
	void clientCanRead();
};
