#include "UnitSyncLib.h"

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QMetaObject>
#include <QPainter>
#include <QTimer>

#define rgb16_size 2 /* QImage::Format_RGB16 take 2 bytes to store one color value */
#define rgb32_size 4 /* QImage::Format_RGB32 take 4 bytes to store one color value */

#define max_mip_level 10

static UnitSyncLib* _lib = 0;

UnitSyncLib::UnitSyncLib()
{

}

UnitSyncLib::~UnitSyncLib()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UnitSyncLib::SetSpringDataDirectory( const QString& springDataDirectory )
{
	_fileManager.SetDataDirectory( springDataDirectory, UnitSync_MultiEngine );
}

void UnitSyncLib::SetUnitSyncDirectory( const QString& unitSyncDirectory )
{
	_fileManager.SetDataDirectory( unitSyncDirectory, UnitSync_SingleEngine );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Main info functions -----------------------------------------------------------------------------
QString UnitSyncLib::SpringVersionFull()
{
	if( !CheckUnitSyncLoaded() )
		return QString();

	return QString( _data._springVersion ).append( "." ).append( _data._springVersionPatchset );
}

QString UnitSyncLib::SpringVersion()
{
	if( !CheckUnitSyncLoaded() )
		return QString();

	return _data._springVersion;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Map functions -----------------------------------------------------------------------------------
QList<QString> UnitSyncLib::GetMapsList()
{
	QList<QString> result;
	if( !CheckUnitSyncLoaded() )
		return result;

	result = _data._mapIndexByName.keys();
	qSort( result );

	return result;
}

uint UnitSyncLib::GetMapHash( const QString& mapName )
{
	if( !CheckUnitSyncLoaded() )
		return 0;

	return _data._mapChecksumByName.value( mapName );
}

QList<SpringOption> UnitSyncLib::GetMapOptions( const QString& mapName )
{
	QList<SpringOption> result;
	if( !CheckUnitSyncLoaded() )
		return result;

	if( _data._mapOptionsByName.contains( mapName ) )
		return _data._mapOptionsByName[ mapName ];

	DEBUG_PRINT2( "UNITSYNC :: GetMapOptions( %s )", qPrintable( mapName ) );
	int mapOptionCount = m_GetMapOptionCount( qPrintable( mapName ) );
	DEBUG_PRINT3( "UNITSYNC :: GetMapOptions( %s ) = %i", qPrintable( mapName ), mapOptionCount );

	result = GetOptionList( mapOptionCount );
	DEBUG_PRINT2( "UNITSYNC :: GetMapOptions( %s ) DONE", qPrintable( mapName ) );

	_data._mapOptionsByName[ mapName ] = result;

	return result;
}

MapInfo UnitSyncLib::GetMinimapInfo( const QString& name )
{
	MapInfo info = {};

	if( !CheckUnitSyncLoaded() )
		return info;


	int mapIndex = _data._mapIndexByName.value( name, -1 );
	if( mapIndex == -1 )
	{
		return info;
	}

	info.Author = m_GetMapAuthor( mapIndex );
	info.Description = m_GetMapDescription( mapIndex );

	info.TidalStrength = m_GetMapTidalStrength( mapIndex );
	info.Gravity = m_GetMapGravity( mapIndex );

	info.MaxMetal = -1;
	info.ExtractorRadius = -1;

	int resourceCount = m_GetMapResourceCount( mapIndex );
	for( int i = 0; i < resourceCount; ++i )
	{
		QString resourceName = m_GetMapResourceName( mapIndex, i );
		if( resourceName.toLower() == "metal" )
		{
			info.MaxMetal = m_GetMapResourceMax( mapIndex, i );
			info.ExtractorRadius = m_GetMapResourceExtractorRadius( mapIndex, i );

			break;
		}
	}

	info.MinWind = m_GetMapWindMin( mapIndex );
	info.MaxWind = m_GetMapWindMax( mapIndex );

	info.Width  = m_GetMapWidth ( mapIndex );
	info.Height = m_GetMapHeight( mapIndex );

	int posCount = m_GetMapPosCount( mapIndex );
	for ( int i = 0; i < posCount; ++i )
	{
		int x = m_GetMapPosX( mapIndex, i );
		int y = m_GetMapPosZ( mapIndex, i );
		info.StartPositions.append( QPoint( x, y ) );
	}

	return info;
}

QImage UnitSyncLib::GetTerrain( const QString& mapName, int mapX, int mapY, MipLevelEnum mipLevel )
{
	if( !CheckUnitSyncLoaded() )
		return QImage();

	const uint width  = 1 << ( max_mip_level - (int)mipLevel );
	const uint height = 1 << ( max_mip_level - (int)mipLevel );

	DEBUG_PRINT3( "UNITSYNC :: m_GetMinimap(%s, %i)", qPrintable( mapName ), mipLevel );
	/// @note
	/// [a1983] should not be deleted, because imagePtr point to static memory
	const uchar* imagePtr =
			reinterpret_cast< uchar* >( m_GetMinimap( mapName.toLocal8Bit(), mipLevel ) );

	DEBUG_PRINT2( "UNITSYNC :: m_GetMinimap: %p", imagePtr );
	QImage mapImage( imagePtr, width, height, QImage::Format_RGB16 );
	DEBUG_PRINT3( "UNITSYNC :: Minimap image: %i x %i", mapImage.width(), mapImage.height() );

	if( mapX == 0 || mapY == 0 )
	{
		qWarning( "UNITSYNC :: MINIMAP :: ERROR, mapX or mapY can't be 0" );
		return QImage();
	}

	if( mapX == mapY )
	{
		DEBUG_PRINT( "UNITSYNC :: Minimap copy" );
		return mapImage.copy();
	}

	uint realW, realH;
	if( mapX > mapY )
	{
		realW = width;
		realH = height * mapY / mapX;
	}
	else
	{
		realW = width * mapX / mapY;
		realH = height;
	}

	QImage scaled = mapImage.scaled( realW, realH );
	DEBUG_PRINT3( "UNITSYNC :: Minimap scale: %i x %i", realW, realH );

	return scaled;
}

QImage UnitSyncLib::GetHeightMap( const QString& mapFileName, MipLevelEnum mipLevel )
{
	if( !CheckUnitSyncLoaded() )
		return QImage();

	int width, height;
	m_GetInfoMapSize( mapFileName.toLocal8Bit(), "height", &width, &height );

	QImage mapImage( width, height, QImage::Format_RGB32 );
	uint* rgbImage = reinterpret_cast< uint* >( mapImage.bits() );

	int mapSize = width * height;
	ushort* heightMap = new ushort[ mapSize ];
	{
		m_GetInfoMap( mapFileName.toLocal8Bit(), "height", heightMap, 2 );

		quint8* value;
		const int shortMax = 0xFFFF;
		for( int i = 0; i < mapSize; ++i )
		{
			value = reinterpret_cast< quint8* >( rgbImage );
			++rgbImage;

			*( value ) = heightMap[ i ] / ( float )shortMax * 0xFF;
			*( value + 1 ) = *value;
			*( value + 2 ) = *value;
			*( value + 3 ) = 0;
		}
	}
	delete[] heightMap;

	int scaledW = 512; int scaledH = 512;
	if( mipLevel == mip1024x1024 ) {
		scaledW = 1024; scaledH = 1024;
	}

	if( width > height ) {
		scaledH = scaledH * height / width;
	}
	else {
		scaledW = scaledW * width / height;
	}

	return mapImage.scaled( scaledW, scaledH );
}

QImage UnitSyncLib::GetMetalMap( const QString& mapName, const QImage& terrain )
{
	if( !CheckUnitSyncLoaded() )
		return QImage();

	int width, height;
	m_GetInfoMapSize( mapName.toLocal8Bit(), "metal", &width, &height );

	QImage mapImage( width, height, QImage::Format_ARGB32 );
	uint* rgbImage = reinterpret_cast< uint* >( mapImage.bits() );

	int mapSize = width * height;
	uchar* metallMap = new uchar[ mapSize ];
	{
		m_GetInfoMap( mapName.toLocal8Bit(), "metal", metallMap, 1 );

		QColor color( QColor::fromHsv( 0x96, 0xFF, 0xFF ) );
		for( int i = 0; i < mapSize; ++i )
		{
			color.setAlpha( metallMap[ i ] );
			rgbImage[ i ] = color.rgba();
		}
	}
	delete[] metallMap;

	QPainter painter( &mapImage );
	{
		painter.setCompositionMode( QPainter::CompositionMode_DestinationOver );
		painter.fillRect( mapImage.rect(), QBrush( QColor( 0, 0, 0, 0xB4 ) ) );
		painter.drawImage( mapImage.rect(), terrain );
		painter.end();
	}

	return mapImage.scaled( terrain.size() );
}
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
///// GAMES functions ------------------------------------------------------------------------------
QList<QString> UnitSyncLib::GetGamesList()
{
	QList<QString> result;
	if( !CheckUnitSyncLoaded() )
		return result;

	result = _data._modChecksumByName.keys();
	qSort( result );

	return result;
}

uint UnitSyncLib::GetGameHash( const QString& gameName )
{
	uint result = 0;
	if( !CheckUnitSyncLoaded() )
		return result;

	result = _data._modChecksumByName.value( gameName );

	return result;
}

QList<SideInfo> UnitSyncLib::GetGameSides( const QString& gameName )
{
	QList<SideInfo> result;
	if( !CheckGameArchiveLoaded( gameName ) )
		return result;

	result = _data._sideInfoCache;

	return result;
}

QList< SkirmishAiInfo > UnitSyncLib::GetGameAIList( const QString& gameName )
{
	QList<SkirmishAiInfo> result;
	if( !CheckGameArchiveLoaded( gameName ) )
		return result;

	result = _data._skirmishAiInfo;

	return result;
}

QList<QString> UnitSyncLib::GetGameValidMaps( const QString& gameName )
{
	QList<QString> result;
	if( !CheckGameArchiveLoaded( gameName ) )
		return result;

	result = _data._validMaps.toList();
	qSort( result );

	return result;
}

QList<SpringOption> UnitSyncLib::GetGameOptions( const QString& gameName )
{
	QList<SpringOption> result;
	if( !CheckGameArchiveLoaded( gameName ) )
		return result;

	if( _data._modOptionsByName.contains( gameName ) )
		return _data._modOptionsByName[ gameName ];

	DEBUG_PRINT2( "UNITSYNC :: GetModOptionCount( %s )", qPrintable( gameName ) );
	int modOptionCount = m_GetModOptionCount();
	DEBUG_PRINT3( "UNITSYNC :: GetModOptionCount( %s ) = %i", qPrintable( gameName ), modOptionCount );

	result = GetOptionList( modOptionCount );
	DEBUG_PRINT2( "UNITSYNC :: GetModOptions( %s ) DONE", qPrintable( gameName ) );

	_data._modOptionsByName[ gameName ] = result;

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///// Helpers --------------------------------------------------------------------------------------
bool UnitSyncLib::CheckUnitSyncLoaded()
{
	return UnitSyncLibApi::IsLoaded( false ) || Load();
}

bool UnitSyncLib::CheckGameArchiveLoaded( const QString& gameName )
{
	return CheckUnitSyncLoaded() && SetCurrentGame( gameName );
}

bool UnitSyncLib::Load()
{
	if( !UnitSyncLibApi::LoadLibrary( _fileManager.UnitSyncFile() ) )
		return false;

	if( _fileManager.Mode() == UnitSync_MultiEngine )
		SetupDataDirectory();

	if( !UnitSyncLibApi::Initialize() )
		return false;

	InitSpringVersion();
	InitGames();
	InitMaps();

	return true;
}

bool UnitSyncLib::Reload()
{
	if( UnitSyncLibApi::IsLoaded( false ) )
	{
		DEBUG_PRINT ("UNITSYNC :: RemoveAllArchives()");
		m_RemoveAllArchives();

		DEBUG_PRINT ("UNITSYNC :: UnInit()");
		m_UnInit();

		UnitSyncLibApi::UnloadLibrary();

		_data.Clear();
	}
	return Load();
}

bool UnitSyncLib::SetCurrentGame( const QString& gameName )
{
	if( _data._currentModName == gameName )
		return true;

	_data.Clear( UnitSyncData::OnlyGame );

	int index = m_GetPrimaryModIndex( qPrintable( gameName ) );
	DEBUG_PRINT3( "UNITSYNC :: GetPrimaryModIndex(%s) = %i", qPrintable( gameName ), index);

	m_RemoveAllArchives();
	DEBUG_PRINT ( "UNITSYNC :: RemoveAllArchives()");

	if( index < 0 )
		return false;

	const char* archives = m_GetPrimaryModArchive( index );
	DEBUG_PRINT3( "UNITSYNC :: GetPrimaryModArchive(%i) = %s", index, archives);

	m_AddAllArchives( archives );
	DEBUG_PRINT2( "UNITSYNC :: AddAllArchives(%s)", archives);

	_data._currentModName = gameName;
//	_data._currentModDependcies = GetModDependencies();

	DEBUG_PRINT ( "UNITSYNC :: Taking Skirmish AIs" );
	InitSkirmishAICache();

	DEBUG_PRINT ( "UNITSYNC :: Taking Valid Maps" );
	InitValidMapsCache();

	DEBUG_PRINT ( "UNITSYNC :: Taking Sides and their Icons" );
	InitSideCache();

	DEBUG_PRINT ( "UNITSYNC :: Set Current Game DONE" );
	return true;
}

void UnitSyncLib::SetupDataDirectory()
{
	m_SetSpringConfigFile( qPrintable( _fileManager.SpringConfigFile() ) );
	m_SetSpringConfigString( "SpringData", qPrintable( _fileManager.DataDirectory() ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Init spring cache -------------------------------------------------------------------------------
void UnitSyncLib::InitSpringVersion()
{
	const char* springVersion = m_GetSpringVersion();
	DEBUG_PRINT2( "UNITSYNC :: GetSpringVersion() = %s", springVersion );

	_data._springVersion = QString( springVersion );

	const char* springPatchset = m_GetSpringVersionPatchset();
	DEBUG_PRINT2( "UNITSYNC :: GetSpringVersionPatchset() = %s", springVersion );

	_data._springVersionPatchset = QString( springPatchset );
}

void UnitSyncLib::InitGames()
{
	int modCount = m_GetPrimaryModCount();
	DEBUG_PRINT2( "UNITSYNC :: GetPrimaryModCount() = %i", modCount );

	for( int i = 0; i < modCount; ++i )
	{
		int infoCount = m_GetPrimaryModInfoCount( i );
		QHash< QString, QString > modInfo = BuildInfo( infoCount );

		uint checkSum = m_GetPrimaryModChecksum( i );
		_data._modChecksumByName.insert( modInfo.value( "name" ), checkSum );
	}
}

void UnitSyncLib::InitMaps()
{
	int mapCount = m_GetMapCount();
	DEBUG_PRINT2( "UNITSYNC :: GetMapCount() = %i", mapCount );

	for( int i = 0; i < mapCount; ++i )
	{
		int checkSum = m_GetMapChecksum( i );

		const char* name = m_GetMapName( i );
		if( name )
		{
			_data._mapIndexByName.insert( name, i );
			_data._mapChecksumByName.insert( name, checkSum );
		}
	}

	DEBUG_PRINT( "UNITSYNC :: EXIT SUCCESS :: void UnitSyncLib::InitMapNamesCache()" )
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Init current game cache -------------------------------------------------------------------------
void UnitSyncLib::InitSkirmishAICache()
{
	int aiCount = m_GetSkirmishAICount();
	DEBUG_PRINT2( "UNITSYNC :: GetSkirmishAICount() %i", aiCount );

	for (int aiIndex = 0; aiIndex < aiCount; aiIndex++)
	{
		int infoItemCount = m_GetSkirmishAIInfoCount (aiIndex);

		SkirmishAiInfo aiInfo;
		{
			QHash < QString, QString > springInfo;
			springInfo = BuildInfo( infoItemCount );

			aiInfo.ShortName =		springInfo.value( "shortName" );
			aiInfo.Name =			springInfo.value( "name" );
			aiInfo.Description =	springInfo.value( "description" );
			aiInfo.URL =			springInfo.value( "url" );
			aiInfo.Version =		springInfo.value( "version" );

			_data._skirmishAiInfo.append( aiInfo );
		}
	}
}

void UnitSyncLib::InitValidMapsCache()
{
	int count = m_GetModValidMapCount();
	DEBUG_PRINT2( "UNITSYNC :: GetModValidMapCount() = %i", count );

	for ( int index = 0; index < count; index++ )
	{
		QString mapName = m_GetModValidMap( index );
		DEBUG_PRINT3( "UNITSYNC :: m_GetModValidMap( %i ) = %s", index, qPrintable( mapName ) );

		_data._validMaps.insert( mapName );
	}
}

void UnitSyncLib::InitSideCache()
{
	int sideCount = m_GetSideCount();
	DEBUG_PRINT2 ( "UNITSYNC :: GetSideCount() = %i", sideCount );

	for ( int i = 0; i < sideCount; i++ )
	{
		QString name = m_GetSideName( i );
		DEBUG_PRINT3 ( "UNITSYNC :: GetSideName(%i) = %s", i, qPrintable( name ) );

		_data._sideInfoCache.append( SideInfo( name, GetSideIcon( name ) ) );
	}
}

QImage UnitSyncLib::GetSideIcon ( const QString &sideName )
{
	DEBUG_PRINT2 ( "UNITSYNC :: Try get side icon for %s", qPrintable( sideName ) );

	QString filePath;
	QByteArray sideIconArray;

	QStringList extensions = QStringList() << "png" << "bmp";
	for( int i = 0; i < extensions.size(); ++i ) {
		filePath = QString("sidepics/%1.%2").arg( sideName ).arg( extensions[ i ] );
		sideIconArray = GetVFSFile( filePath );
		if( sideIconArray.size() > 0 )
			break;
	}

	QImage rawImage;
	rawImage.loadFromData( sideIconArray );

	return rawImage;
}

QByteArray UnitSyncLib::GetVFSFile( const QString &fileName )
{
	QByteArray result;

	if( !CheckUnitSyncLoaded() )
		return result;

	int fileID = m_OpenFileVFS( qPrintable( fileName ) );
	DEBUG_PRINT3 ( "UNITSYNC :: OpenFileVFS (%s) = %i", qPrintable( fileName ), fileID );

	if ( !fileID )
		return result;

	int fileSize = m_FileSizeVFS( fileID );
	DEBUG_PRINT3 ( "UNITSYNC :: FileSizeVFS (%i) = %i", fileID, fileSize );

	if ( !fileSize )
	{
		m_CloseFileVFS( fileID );
		return result;
	}

	result.resize( fileSize );

	m_ReadFileVFS( fileID, result.data(), fileSize );
	DEBUG_PRINT3 ( "UNITSYNC :: ReadFileVFS (%i, %i)", fileID, fileSize );

	m_CloseFileVFS( fileID );

	return result;
}

QHash< QString, QString > UnitSyncLib::BuildInfo( int infoItemCount )
{
	QHash< QString, QString > result;
	QString infoKey;
	QString infoValue;
//	QString infoDescription;

	for ( int infoIndex = 0; infoIndex < infoItemCount; infoIndex++ )
	{
		infoKey = QString( m_GetInfoKey( infoIndex ) );
//		qDebug ("UNITSYNC :: m_GetInfoKey (%i) = %s", infoIndex, qPrintable(infoKey));
		QString type = m_GetInfoType( infoIndex );
		if( type == "string" )
			infoValue = QString( m_GetInfoValueString( infoIndex ) );
		else if( type == "integer" )
			infoValue = QString::number( m_GetInfoValueInteger( infoIndex ) );
		else if( type == "float" )
			infoValue = QString::number( m_GetInfoValueFloat( infoIndex ) );
		else if( type == "bool" )
			infoValue = m_GetInfoValueBool( infoIndex ) ? "true" : "false";
		else {
			qWarning( "UnitSyncLib::BuildInfo UNKNOWN TYPE!" );
			infoValue = "";
		}

//		qDebug ("UNITSYNC :: m_GetInfoValue (%i) = %s", infoIndex, qPrintable(infoValue));
//		infoDescription = QString( m_GetInfoDescription( infoIndex ) );
//		qDebug ("UNITSYNC :: m_GetInfoDescription (%i) = %s", infoIndex, qPrintable(infoValue));

		result.insert( infoKey, infoValue );
	}

	return result;
}

QList<SpringOption> UnitSyncLib::GetOptionList(int count)
{
	QList<SpringOption> result;
	for( int i = 0; i < count; ++i )
		result.append( GetOption( i ) );
	return result;
}

SpringOption UnitSyncLib::GetOption( int index )
{
//	DEBUG_PRINT2( "UNITSYNC :: GetOption( %i )", index );

	SpringOption result;
	result[ SOI_KEY ]			= m_GetOptionKey( index );
	result[ SOI_NAME ]			= m_GetOptionName( index );
	result[ SOI_SECTION ]		= m_GetOptionSection( index );
	result[ SOI_STYLE ]			= m_GetOptionStyle( index );
	result[ SOI_DESCRIPTION ]	= m_GetOptionDesc( index );
	result[ SOI_TYPE ]			= m_GetOptionType( index );

	switch ( result[ SOI_TYPE ].toInt() ) {
	case OT_BOOLEAN:
		result[ SOI_DEFAULT ] = m_GetOptionBoolDef( index );
		break;

	case OT_LIST: {
		result[ SOI_DEFAULT ] = m_GetOptionListDef( index );
		int optionListCount = m_GetOptionListCount( index );
		QVariantList list;
		for( int i = 0; i < optionListCount; ++i ) {
			QVariantList item;
			item.append( m_GetOptionListItemKey( index, i ) );
			item.append( m_GetOptionListItemName( index, i ) );
			item.append( m_GetOptionListItemDesc( index, i ) );
			list.append( QVariant( item ) );
		}
		result[ SOI_LIST ] = list;
		break;
	}
	case OT_FLOAT:
		result[ SOI_DEFAULT ]		= m_GetOptionNumberDef( index );
		result[ SOI_NUMBER_MIN ]	= m_GetOptionNumberMin( index );
		result[ SOI_NUMBER_MAX ]	= m_GetOptionNumberMax	( index );
		result[ SOI_NUMBER_STEP ]	= m_GetOptionNumberStep	( index );
		break;

	case OT_STRING:
		result[ SOI_DEFAULT ] = m_GetOptionStringDef( index );
		result[ SOI_STRING_MAX_LENGHT ]	= m_GetOptionStringMaxLen( index );
		break;

	case OT_SECTION:
		break;

	default:
		DEBUG_PRINT3( "UNITSYNC :: UNKNOWN OPTION TYPE ( %s :: %i )",
					 qPrintable( result[ SOI_KEY ].toString() ),
					 result[ SOI_TYPE ].toInt() );
		break;
	}

//	DEBUG_PRINT2( "UNITSYNC :: GetOption( %i ) DONE", index );
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
UnitSyncLib* UnitSync()
{
	if( !_lib )
		_lib = new UnitSyncLib();

	return _lib;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
