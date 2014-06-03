#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QMap>

class QTcpServer;
class RequestProcessor;

class ServerManager : public QObject
{
	Q_OBJECT

public:
	ServerManager( RequestProcessor* builder );
	~ServerManager();

	bool Initialize( quint16 port );

	void ProcessRequest( const QByteArray& data );

private slots:
	void DoNewConnection();

private:
	QByteArray GetRequest( const QByteArray& data );

private:
	QTcpServer* _server;
	RequestProcessor* _processor;
};

#endif // SERVERMANAGER_H
