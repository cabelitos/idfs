#include <QCoreApplication>
#include <QCommandLineParser>
#include "client_socket.hh"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCommandLineParser parser;


	QCoreApplication::setApplicationName("IDFS client");
	QCoreApplication::setApplicationVersion("1.0");
	parser.setApplicationDescription("IDFS client");
	parser.addHelpOption();
	parser.addVersionOption();

	parser.addPositionalArgument("command", "The command to be executed");
	parser.addPositionalArgument("args", "Command arguments");

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
	QStringList args = parser.positionalArguments();

	port = parser.value(portOption).toUInt(&ok);
	host = parser.value(hostOption);

	if (args.size() < 2 || !ok)
		parser.showHelp(-1);

	ClientSocket socket;
	socket.connectToHost(host, port);
	socket.sendCommand(args);
	return app.exec();
}
