#include "UnitSyncMapsProcessor.h"

#include <QBuffer>

#include "Logic/UnitSyncLib/UnitSyncLib.h"
#include "ProcessorsTools.h"

QByteArray MapsChecksumRequest	= "Checksum";
QByteArray MapInfoRequest		= "Info";
QByteArray TerrainRequest		= "Image Terrain";
QByteArray HeightRequest		= "Image Height";
QByteArray MetalRequest			= "Image Metal";
QByteArray MapsOptionsRequest	= "Options";
QByteArray MapsOptionsListRequest	= "OptionsList";

UnitSyncMapsProcessor::UnitSyncMapsProcessor()
{
}

void AppendHtmlLink( const QByteArray& map , QByteArray* html )
{
	html->append( "<a href=\"/Maps/" ).append( map ).append( "\">" ).append( map ).append( "</a><br>" );
}

bool UnitSyncMapsProcessor::BuildResponse()
{
	QList<QString> maps = UnitSync()->GetMapsList();
	QByteArray html;

	for( int i = 0; i < maps.size(); ++i ) {
		AppendHtmlLink( maps[ i ].toUtf8(), &html );
	}

	SetResponse( html, Response_Html );
	return true;
}

QByteArray BuildMapMenu( const QByteArray& map ) {
	QByteArray prefix = "<a href=\"/Maps/" + map + "/";
	QList< QByteArray > items;
	items << MapsChecksumRequest << MapInfoRequest << TerrainRequest << HeightRequest << MetalRequest
		  << MapsOptionsRequest << MapsOptionsListRequest;

	return ProcessorsTools::BuildMenu( prefix, items );
}

bool UnitSyncMapsProcessor::BuildSubResponse( const QByteArray& subRequest, const QByteArray& data )
{
	if( data.isEmpty() ) {
		SetResponse( BuildMapMenu( subRequest ), Response_Html );
		return true;
	}

	QList< QByteArray > cmdList = data.split( L'/' );
	if( cmdList.size() < 2 ) {
		SetResponse( "Wrong data", Response_PlainText );
		return true;
	}

	const QByteArray cmd = cmdList[ 1 ];
	if( cmd == MapsChecksumRequest ) {
		SetResponse( QByteArray::number( UnitSync()->GetMapHash( subRequest ) ), Response_PlainText );
		return true;
	}
	if( cmd == MapInfoRequest ) {
		return ProcessInfoRequest( subRequest );
	}
	if( cmd == TerrainRequest ) {
		return ProcessTerrainRequest( subRequest );
	}
	if( cmd == HeightRequest ) {
		return ProcessHeightRequest( subRequest );
	}
	if( cmd == MetalRequest ) {
		return ProcessMetalRequest( subRequest );
	}
	if( cmd == MapsOptionsRequest ) {
		return ProcessOptionsRequest( subRequest, cmdList );
	}
	if( cmd == MapsOptionsListRequest ) {
		SetResponse( ProcessorsTools::BuildOptionsList( UnitSync()->GetMapOptions( subRequest ) ), Response_PlainText );
		return true;
	}

	SetResponse( "Wrong map request", Response_PlainText );
	return true;
}

bool UnitSyncMapsProcessor::ProcessInfoRequest( const QByteArray& map )
{
	QByteArray result;
	MapInfo info = UnitSync()->GetMinimapInfo( map );
	result.append( ProcessorsTools::ReplaceNewLine( info.Author ) ).append( "\n" );
	result.append( ProcessorsTools::ReplaceNewLine( info.Description ) ).append( "\n" );

	result.append( QByteArray::number( info.TidalStrength ) ).append( "\n" );
	result.append( QByteArray::number( info.Gravity ) ).append( "\n" );

	result.append( QByteArray::number( info.MaxMetal ) ).append( "\n" );
	result.append( QByteArray::number( info.ExtractorRadius ) ).append( "\n" );

	result.append( QByteArray::number( info.MinWind ) ).append( "\n" );
	result.append( QByteArray::number( info.MaxWind ) ).append( "\n" );

	result.append( QByteArray::number( info.Width ) ).append( "\n" );
	result.append( QByteArray::number( info.Height ) ).append( "\n" );

	SetResponse( result, Response_PlainText );
	return true;
}

bool UnitSyncMapsProcessor::ProcessTerrainRequest( const QByteArray& map )
{
	QByteArray result;
	MapInfo info = UnitSync()->GetMinimapInfo( map );
	QImage terrain = UnitSync()->GetTerrain( map, info.Width, info.Height );
	result = ProcessorsTools::FromImage( terrain );
	SetResponse( result, Response_Image );
	return true;
}

bool UnitSyncMapsProcessor::ProcessHeightRequest( const QByteArray& map )
{
	QByteArray result;
	QImage height = UnitSync()->GetHeightMap( map );
	result = ProcessorsTools::FromImage( height );
	SetResponse( result, Response_Image );
	return true;
}

bool UnitSyncMapsProcessor::ProcessMetalRequest( const QByteArray& map )
{
	QByteArray result;
	MapInfo info = UnitSync()->GetMinimapInfo( map );
	QImage terrain = UnitSync()->GetTerrain( map, info.Width, info.Height );
	QImage metal = UnitSync()->GetMetalMap( map, terrain );
	result = ProcessorsTools::FromImage( metal );
	SetResponse( result, Response_Image );
	return true;
}

QByteArray BuildMapOptionsList( const QByteArray& map ) {
	QByteArray prefix = "<a href=\"/Maps/" + map + "/" + MapsOptionsRequest +"/";

	QList< SpringOption > options = UnitSync()->GetMapOptions( map );
	QList< QByteArray > items;
	for( int i = 0; i < options.size(); ++i ) {
		const SpringOption& option = options[ i ];
		items.append( option[ SOI_KEY ].toString().toUtf8() );
	}

	return ProcessorsTools::BuildMenu( prefix, items );
}

bool UnitSyncMapsProcessor::ProcessOptionsRequest( const QByteArray& map, const QList<QByteArray>& subRequest )
{
	if( subRequest.size() == 2 ) {
		SetResponse( BuildMapOptionsList( map ), Response_Html );
		return true;
	}
	if( subRequest.size() == 3 ) {
		QList< SpringOption > options = UnitSync()->GetMapOptions( map );
		SetResponse( ProcessorsTools::BuildOption( subRequest[ 2 ], options ), Response_PlainText );
		return true;
	}

	SetResponse( "Wrong map options request", Response_Html );
	return true;
}

