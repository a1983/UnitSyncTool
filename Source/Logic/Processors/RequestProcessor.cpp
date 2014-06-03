#include "RequestProcessor.h"

RequestProcessor::RequestProcessor()
{
}

RequestProcessor::~RequestProcessor()
{
}

QByteArray GetSubRequestName( const QByteArray& data )
{
	int endOfName = data.indexOf( L'/', 1 );
	if( endOfName == -1 )
		return data.mid( 1 );

	return data.mid( 1, endOfName - 1 );
}

bool RequestProcessor::ProcessRequest( const QByteArray& data )
{
	if( data.size() < 2 ) {
		return BuildResponse();
	}

	QByteArray subRequestName = GetSubRequestName( data );
	QByteArray subRequestData;
	if( subRequestName.size() < ( data.size() - 1 ) )
		subRequestData = data.mid( subRequestName.size() + 1 );

	return BuildSubResponse( subRequestName, subRequestData );
}

void RequestProcessor::SetResponse( const QByteArray& data, ResponseType type )
{
	_responseData = data;
	_responseType = type;
}

QByteArray RequestProcessor::Response() const
{
	return _responseData;
}

ResponseType RequestProcessor::Type() const
{
	return _responseType;
}


