#include "UnitSyncGamesProcessor.h"

#include "Logic/UnitSyncLib/UnitSyncLib.h"

#include "ProcessorsTools.h"

QByteArray GameChecksumRequest	= "Checksum";
QByteArray GameSidesRequest		= "Sides";
QByteArray GameSidesListRequest	= "SidesList";
QByteArray SideIconRequest		= "Icon";
QByteArray AIRequest			= "AI";
QByteArray AIListRequest		= "AI List";
QByteArray ValidMapsRequest		= "Valid maps";
QByteArray GameOptionsRequest	= "Options";
QByteArray GamesOptionsListRequest = "OptionsList";


UnitSyncGamesProcessor::UnitSyncGamesProcessor()
{
}

bool UnitSyncGamesProcessor::BuildResponse()
{
	QList<QString> games = UnitSync()->GetGamesList();
	QByteArray html;

	for( int i = 0; i < games.size(); ++i ) {
		QByteArray game = games[ i ].toUtf8();
		html.append( "<a href=\"/Games/" ).append( game ).append( "\">" ).append( game ).append( "</a><br>" );
	}

	SetResponse( html, Response_Html );
	return true;
}

QByteArray BuildGameMenu( const QByteArray& game ) {
	QByteArray prefix = "<a href=\"/Games/" + game + "/";
	QList< QByteArray > items;
	items << GameChecksumRequest
		  << GameSidesRequest	<< GameSidesListRequest
		  << AIRequest			<< AIListRequest
		  << ValidMapsRequest
		  << GameOptionsRequest << GamesOptionsListRequest;

	return ProcessorsTools::BuildMenu( prefix, items );
}

bool UnitSyncGamesProcessor::BuildSubResponse( const QByteArray& subRequest, const QByteArray& data )
{
	if( data.isEmpty() ) {
		SetResponse( BuildGameMenu( subRequest ), Response_Html );
		return true;
	}

	QList< QByteArray > cmdList = data.split( L'/' );
	if( cmdList.size() < 2 ) {
		SetResponse( "Wrong data", Response_Html );
		return true;
	}

	const QByteArray cmd = cmdList[ 1 ];
	if( cmd == GameChecksumRequest ) {
		SetResponse( QByteArray::number( UnitSync()->GetGameHash( subRequest ) ), Response_Html );
		return true;
	}
	else if( cmd == GameSidesRequest ) {
		return ProcessSideRequest( subRequest, cmdList );
	}
	else if( cmd == GameSidesListRequest ) {
		return ProcessSideListRequest( subRequest );
	}
	else if( cmd == AIRequest ) {
		return ProcessAIRequest( subRequest, cmdList );
	}
	else if( cmd == AIListRequest ) {
		return ProcessAIListRequest( subRequest );
	}
	else if( cmd == ValidMapsRequest ) {
		return ProcessValidMapsRequest( subRequest );
	}
	else if( cmd == GameOptionsRequest ) {
		return ProcessOptionsRequest( subRequest, cmdList );
	}
	if( cmd == GamesOptionsListRequest ) {
		SetResponse( ProcessorsTools::BuildOptionsList( UnitSync()->GetGameOptions( subRequest ) ), Response_PlainText );
		return true;
	}

	SetResponse( "Wrong game request", Response_Html );
	return true;
}

QByteArray BuildSideHtmlList( const QByteArray& game ) {
	QByteArray prefix = "<a href=\"/Games/" + game + "/" + GameSidesRequest +"/";

	QList< SideInfo > sides = UnitSync()->GetGameSides( game );
	QList< QByteArray > items;
	for( int i = 0; i < sides.size(); ++i ) {
		const SideInfo& side = sides[ i ];
		items.append( side.Name.toUtf8() );
	}

	return ProcessorsTools::BuildMenu( prefix, items );
}

QByteArray BuildSideMenu( const QByteArray& game, const QByteArray& side ) {
	QByteArray prefix = "<a href=\"/Games/" + game + "/" + GameSidesRequest +"/" + side + "/";
	QList< QByteArray > items;
	items << SideIconRequest;
	return ProcessorsTools::BuildMenu( prefix, items );
}

QByteArray BuildSideIcon( const QByteArray& game, const QByteArray& side ) {
	QByteArray result;

	QList< SideInfo > items = UnitSync()->GetGameSides( game );
	int index = -1;
	for( int i = 0; i < items.size(); ++i ) {
		if( items[ i ].Name == side ) {
			index = i;
			break;
		}
	}

	if( index < 0 )
		return result;

	result = ProcessorsTools::FromImage( items[ index ].Icon );

	return result;
}

bool UnitSyncGamesProcessor::ProcessSideRequest( const QByteArray& game, const QList<QByteArray>& subRequest )
{
	if( subRequest.size() == 2 ) {
		SetResponse( BuildSideHtmlList( game ), Response_Html );
		return true;
	}
	else if( subRequest.size() == 3 ) {
		SetResponse( BuildSideMenu( game, subRequest[ 2 ] ), Response_Html );
		return true;
	}
	else if( subRequest.size() == 4 && subRequest[ 3 ] == SideIconRequest ) {
		SetResponse( BuildSideIcon( game, subRequest[ 2 ] ), Response_Image );
		return true;
	}

	SetResponse( "Wrong game side request", Response_Html );
	return true;
}

bool UnitSyncGamesProcessor::ProcessSideListRequest( const QByteArray& game )
{
	QList< SideInfo > sides = UnitSync()->GetGameSides( game );
	QByteArray response;
	for( int i = 0; i < sides.size(); ++i ) {
		const SideInfo& side = sides[ i ];
		response.append( side.Name.toUtf8() ).append( '\n' );
	}
	SetResponse( response, Response_PlainText );
	return true;
}

QByteArray BuildAIListList( const QByteArray& game ) {
	QByteArray prefix = "<a href=\"/Games/" + game + "/" + AIListRequest +"/";

	QList< SkirmishAiInfo > aiList = UnitSync()->GetGameAIList( game );
	QList< QByteArray > items;
	for( int i = 0; i < aiList.size(); ++i ) {
		const SkirmishAiInfo& ai = aiList[ i ];
		items.append( ai.ShortName.toUtf8() );
	}

	return ProcessorsTools::BuildMenu( prefix, items );
}

QByteArray BuildAIInfo( const QByteArray& game, const QByteArray& ai ) {
	QByteArray result;

	QList< SkirmishAiInfo > items = UnitSync()->GetGameAIList( game );
	int index = -1;
	for( int i = 0; i < items.size(); ++i ) {
		if( items[ i ].ShortName == ai ) {
			index = i;
			break;
		}
	}

	if( index < 0 )
		return result;

	const SkirmishAiInfo& info = items[ index ];
	result
			.append( info.ShortName.toUtf8() )	.append( '\n' )
			.append( info.Name.toUtf8() )		.append( '\n' )
			.append( info.Description.toUtf8() ).append( '\n' )
			.append( info.URL.toUtf8() )		.append( '\n' )
			.append( info.Version.toUtf8() )	.append( '\n' );

	return result;
}

bool UnitSyncGamesProcessor::ProcessAIRequest( const QByteArray& game, const QList<QByteArray>& subRequest )
{
	if( subRequest.size() == 2 ) {
		SetResponse( BuildAIListList( game ), Response_Html );
		return true;
	}
	else if( subRequest.size() == 3 ) {
		SetResponse( BuildAIInfo( game, subRequest[ 2 ] ), Response_PlainText );
		return true;
	}

	SetResponse( "Wrong game AI list request", Response_Html );
	return true;
}

bool UnitSyncGamesProcessor::ProcessAIListRequest( const QByteArray& game )
{
	QList< SkirmishAiInfo > aiList = UnitSync()->GetGameAIList( game );
	QByteArray response;
	for( int i = 0; i < aiList.size(); ++i ) {
		const SkirmishAiInfo& ai = aiList[ i ];
		response.append( ai.ShortName.toUtf8() );
	}
	SetResponse( response, Response_PlainText );
	return true;
}

bool UnitSyncGamesProcessor::ProcessValidMapsRequest( const QByteArray& game )
{
	QByteArray result;

	QList< QString > items = UnitSync()->GetGameValidMaps( game );

	for( int i = 0; i < items.size(); ++i ) {
		result.append( items[ i ] ).append( "<br>" );
	}

	SetResponse( result, Response_Html );
	return true;
}

QByteArray BuildGamesOptionsList( const QByteArray& game ) {
	QByteArray prefix = "<a href=\"/Games/" + game + "/" + GameOptionsRequest +"/";

	QList< SpringOption > options = UnitSync()->GetGameOptions( game );
	QList< QByteArray > items;
	for( int i = 0; i < options.size(); ++i ) {
		const SpringOption& option = options[ i ];
		items.append( option[ SOI_KEY ].toString().toUtf8() );
	}

	return ProcessorsTools::BuildMenu( prefix, items );
}

bool UnitSyncGamesProcessor::ProcessOptionsRequest( const QByteArray& game, const QList<QByteArray>& subRequest )
{
	if( subRequest.size() == 2 ) {
		SetResponse( BuildGamesOptionsList( game ), Response_Html );
		return true;
	}
	if( subRequest.size() == 3 ) {
		QList< SpringOption > options = UnitSync()->GetGameOptions( game );
		SetResponse( ProcessorsTools::BuildOption( subRequest[ 2 ], options ), Response_PlainText );
		return true;
	}

	SetResponse( "Wrong game options request", Response_Html );
	return true;
}
