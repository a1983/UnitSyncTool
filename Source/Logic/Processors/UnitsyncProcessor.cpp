#include "UnitSyncProcessor.h"

#include "UnitSyncGamesProcessor.h"
#include "UnitSyncGamesListProcessor.h"

#include "UnitSyncMapsProcessor.h"
#include "UnitSyncMapsListProcessor.h"

UnitSyncProcessor::UnitSyncProcessor()
{
	AppendSubProcessor( "Games",		new UnitSyncGamesProcessor() );
	AppendSubProcessor( "GamesList",	new UnitSyncGamesListProcessor() );

	AppendSubProcessor( "Maps",			new UnitSyncMapsProcessor() );
	AppendSubProcessor( "MapsList",		new UnitSyncMapsListProcessor() );
}

UnitSyncProcessor::~UnitSyncProcessor()
{
	qDeleteAll( _processors.values() );
}

bool UnitSyncProcessor::BuildResponse()
{
	SetResponse( _html, Response_Html );
	return true;
}

bool UnitSyncProcessor::BuildSubResponse( const QByteArray& subRequest, const QByteArray& data )
{
	RequestProcessor* subProcessor = _processors.value( subRequest );
	if( !subProcessor || !subProcessor->ProcessRequest( data ) ) {
		SetResponse( "Not found", Response_Html );
		return false;
	}

	SetResponse( subProcessor->Response(), subProcessor->Type() );
	return true;
}

void UnitSyncProcessor::AppendSubProcessor( const QByteArray& name, RequestProcessor* processor )
{
	AppendHtmlLink( name );
	AppendSubProcessorToMap( name, processor );
}

void UnitSyncProcessor::AppendHtmlLink( const QByteArray& name )
{
	QByteArray htmlLine;
	htmlLine.append( "<a href=\"" ).append( name ).append( "\">" ).append( name ).append( "</a><br>" );
	_html.append( htmlLine );
}

void UnitSyncProcessor::AppendSubProcessorToMap( const QByteArray& name, RequestProcessor* processor )
{
	_processors.insert( name, processor );
}
