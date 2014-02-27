#include <QCoreApplication>
#include <QHostAddress>
#include <QCommandLineParser>
#include <QDebug>
#include "slave_node.hh"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCommandLineParser parser;
	QCoreApplication::setApplicationName("IDFS slave node");
	QCoreApplication::setApplicationVersion("1.0");
	parser.setApplicationDescription("File storaged node");
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption hostOption(QStringList() << "a" << "address",
		"Host's address (default is localhost).", "address", "localhost");
	parser.addOption(hostOption);
	QCommandLineOption portOption(QStringList() << "p" << "port",
		"Host's port (default is 8001).", "port", "8001");
	parser.addOption(portOption);

	parser.process(app);

	QString host;
	quint16 port;
	bool ok;

	port = parser.value(portOption).toUInt(&ok);
	host = parser.value(hostOption);
	if (!ok)
		port = 8001;

	qDebug() << host;
	qDebug() << port;
	SlaveNode node;
	node.connectToHost(host, port);
	return app.exec();
}