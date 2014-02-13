#include <QCoreApplication>
#include <QHostAddress>
#include "master_node.hh"

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	MasterNode master;
	master.listen(QHostAddress::AnyIPv4, 8001);
	app.exec();
	return 1;
}