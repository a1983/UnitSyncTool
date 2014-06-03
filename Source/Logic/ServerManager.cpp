#include "ServerManager.h"

#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>

#include "Processors/RequestProcessor.h"

#include <QStringList>

QByteArray GetHttpResponse( const QByteArray& data , const ResponseType& type );

ServerManager::ServerManager( RequestProcessor* processor ) :
	_server( new QTcpServer() ),
	_processor( processor )
{
	connect( _server, SIGNAL( newConnection() ), SLOT( DoNewConnection() ) );
}

ServerManager::~ServerManager()
{
	delete _server;
}

bool ServerManager::Initialize( quint16 port )
{
	return _server->listen( QHostAddress::Any, port );
}

void ServerManager::DoNewConnection()
{
	QTcpSocket* socket = _server->nextPendingConnection();

	socket->waitForReadyRead();

	QByteArray request = GetRequest( socket->readAll() );

	if( request.isEmpty() ) {
		qWarning( "Wrong request!, %s", request.data() );
		socket->deleteLater();
		return;
	}

	QUrl urlRequest( request );

	_processor->ProcessRequest( urlRequest.toString().toUtf8() );
	QByteArray response = GetHttpResponse( _processor->Response(), _processor->Type() );
//	qDebug( response.data() );
	socket->write( response );
	socket->waitForBytesWritten();
	socket->deleteLater();
}

QByteArray ServerManager::GetRequest( const QByteArray& data )
{
	int endOfLinePos = data.indexOf( L' ' );
	if( endOfLinePos != -1 ) {
		if( data.left( endOfLinePos ) == "GET" ) {
			int endOfRequestPos = data.indexOf( L' ', endOfLinePos + 1 );
			if( endOfRequestPos != -1 ) {
				return data.mid( endOfLinePos + 1, endOfRequestPos - endOfLinePos - 1 );
			}
		}
	}

	return QByteArray();
}

QString _http11 = "HTTP/1.1 200 Ok\r\n";

QStringList _contentType( QStringList()
						  << "Content-Type: text/html; charset=\"utf-8\"\r\n"
						  << "Content-Type: image/PNG\r\n"
						  << "Content-Type: text/plain\r\n" );

QByteArray GetHttpResponse( const QByteArray& data , const ResponseType& type )
{
	QByteArray result;
	result.append( _http11 );
	result.append( _contentType.value( type ) );
	result.append( QByteArray( "Content-Length: " )
							.append( QByteArray::number( data.size() ) )
							.append( "\r\n" ) );
	result.append( "\r\n" );
	result.append( data );

	return result;
}
