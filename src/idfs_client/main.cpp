#include <QCoreApplication>
#include <QTcpSocket>
#include <QCommandLineParser>
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include "fs_message.hh"

static bool _message_create(QStringList &args, QString command, FsMessage &msg)
{
	msg.messageType = FsMessage::COMMAND;
	msg.hostType = FsMessage::CLIENT_APP;
	msg.timeStamp = QDateTime::currentDateTime();

	if (command == "ls")
		msg.commandType = FsMessage::LS;
	else if (command == "mkdir")
		msg.commandType = FsMessage::MKDIR;
	else if (command == "touch")
		msg.commandType = FsMessage::TOUCH;
	else if (command == "push_file")
	{
		QString path = args.takeFirst();
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Can not open the file:" << path;
			return false;
		}
		msg.fileData = file.readAll();
		msg.commandType = FsMessage::PUSH_FILE;
		QFileInfo info(file);
		msg.args << info.fileName();
		file.close();
	}
	else
		msg.commandType = FsMessage::UNKNOWN_COMMAND;

	msg.args << args;
	return true;
}

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

	QTcpSocket socket;
	socket.connectToHost(host, port);
	socket.waitForConnected();
	QDataStream stream(&socket);
	QString command = args.takeFirst().toLower();
	FsMessage msg;
	if (_message_create(args, command, msg))
	{
		stream << fsMessageSizeGet(msg) << msg;
		socket.flush();
		socket.waitForReadyRead(-1);
		FsMessage resp;
		qint64 size;
		/* todo, loop until all data is received */
		stream >> size >> resp;
		if (!resp.success)
			qDebug() << "Error! - reason:"<< resp.errorMessage;
		else
		{
			qDebug() << "Success!\n";
			if (command == "ls")
				qDebug() << resp.args;
		}
	}
	socket.disconnectFromHost();
	return 0;
}
