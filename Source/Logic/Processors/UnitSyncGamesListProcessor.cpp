#include "UnitSyncGamesListProcessor.h"

#include "Logic/UnitSyncLib/UnitSyncLib.h"

#include "ProcessorsTools.h"

UnitSyncGamesListProcessor::UnitSyncGamesListProcessor()
{
}

bool UnitSyncGamesListProcessor::BuildResponse()
{
	QList<QString> games = UnitSync()->GetGamesList();
	QByteArray text;

	for( int i = 0; i < games.size(); ++i ) {
		QByteArray game = games[ i ].toUtf8();
		text.append( game ).append( "\n" );
	}

	SetResponse( text, Response_PlainText );
	return true;
}

bool UnitSyncGamesListProcessor::BuildSubResponse( const QByteArray& /*subRequest*/, const QByteArray& /*data*/ )
{
	return false;
}
