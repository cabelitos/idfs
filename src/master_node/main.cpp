#include <QCoreApplication>
#include <QHostAddress>
#include <QCommandLineParser>
#include <QDebug>
#include "master_node.hh"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCommandLineParser parser;
	QCoreApplication::setApplicationName("IDFS master node");
	QCoreApplication::setApplicationVersion("1.0");
	parser.setApplicationDescription("Where all file metadata is stored.");
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption portOption(QStringList() << "p" << "port",
		"Host's port (default is 8001).", "port", "8001");
	parser.addOption(portOption);
	parser.process(app);

	bool ok;
	quint16 port = parser.value(portOption).toUInt(&ok);
	if (!ok)
	{
		qDebug() << "Could not parse the port value, using 8001";
		port = 8001;
	}
	MasterNode master;
	master.listen(QHostAddress::AnyIPv4, port);
	app.exec();
	return 1;
}