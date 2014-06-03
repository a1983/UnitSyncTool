#include "UnitSyncMapsListProcessor.h"

#include "Logic/UnitSyncLib/UnitSyncLib.h"

UnitSyncMapsListProcessor::UnitSyncMapsListProcessor()
{
}

bool UnitSyncMapsListProcessor::BuildResponse()
{
	QList<QString> maps = UnitSync()->GetMapsList();
	QByteArray text;

	for( int i = 0; i < maps.size(); ++i ) {
		text.append( maps[ i ].toUtf8() ).append( "\n" );
	}

	SetResponse( text, Response_PlainText );
	return true;
}

bool UnitSyncMapsListProcessor::BuildSubResponse( const QByteArray& /*subRequest*/, const QByteArray& /*data*/ )
{
	return false;
}

