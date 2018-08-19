#include "NetworkKeyValueProvider.h"

NetworkKeyValueProvider::NetworkKeyValueProvider(const QString& host, quint16 port)
	: tcpSocket(new QTcpSocket)
{
	tcpSocket->connectToHost(host, port);
	tcpSocket->waitForConnected();
	if (tcpSocket->state() != QAbstractSocket::ConnectedState)
		throw std::exception("cannot connect to the server");

	strm.setDevice(tcpSocket);
	strm.setVersion(QDataStream::Qt_5_10);
}

NetworkKeyValueProvider::~NetworkKeyValueProvider()
{
	delete tcpSocket;
}

QString NetworkKeyValueProvider::value(const QString& key) const
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = QString("request");
	data["method"] = QString("get");
	data["key"] = key;
	strm << data;
	strm.commitTransaction();

	readResponse();

	return response["value"];
}

void NetworkKeyValueProvider::insert(const QString& key, const QString& val)
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = QString("request");
	data["method"] = QString("put");
	data["key"] = key;
	data["value"] = val;
	strm << data;
	strm.commitTransaction();

	readResponse();
}

void NetworkKeyValueProvider::readResponse() const
{
	response.clear();

	tcpSocket->waitForReadyRead();

	strm.startTransaction();
	strm >> response;
	strm.commitTransaction();

	if (response["type"] != "response")
		throw std::exception("invalid response type from the server");
	if (lastResponseError() != "ok")
		throw std::exception(lastResponseError().toStdString().c_str());
}

void NetworkKeyValueProvider::remove(const QString& key)
{
	strm.startTransaction();
	QMap<QString, QString> data;
	data["type"] = QString("request");
	data["method"] = QString("delete");
	data["key"] = key;
	strm << data;
	strm.commitTransaction();

	readResponse();
}

QString NetworkKeyValueProvider::lastResponseDetails() const
{
	return response["details"];
}

QString NetworkKeyValueProvider::lastResponseError() const
{
	return response["error"];
}
