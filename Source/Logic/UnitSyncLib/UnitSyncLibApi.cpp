#include "UnitSyncLibApi.h"

#include <QCoreApplication>
#include <QProcess>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

UnitSyncLibApi::UnitSyncLibApi()
{
	_unitSyncLib = new QLibrary;
}

UnitSyncLibApi::~UnitSyncLibApi()
{
	_unitSyncLib->unload();
	delete _unitSyncLib;
}

bool UnitSyncLibApi::LoadLibrary( const QString& libraryFile )
{
	QFileInfo fileInfo( libraryFile );

#ifdef Q_OS_WIN
	//  Change current dir cause otherwise windows cant load unitsync
	QString originalDir = QDir::currentPath();
	QDir::setCurrent( fileInfo.absolutePath() );
#endif

	// The next line is very important to load/find the shared libraries used by unitsync.dll
	qApp->addLibraryPath( fileInfo.absolutePath() );

	if( !QFile::exists( libraryFile ) )
	{
		qWarning( "UNITSYNC :: Library was not found or is unusable. %s", libraryFile.toStdString().c_str() );
		return false;
	}
	_unitSyncLib->setFileName( libraryFile );

	_unitSyncLib->load();

	if( !_unitSyncLib->isLoaded() )
	{
		qWarning( "UNITSYNC :: Library not loaded" );
		qWarning( "UNITSYNC :: Error: %s -- %s", qPrintable( _unitSyncLib->errorString() ), qPrintable( libraryFile ) );

		return false;
	}

#define RESOLVE( function )												\
	m_ ## function = ( function )_unitSyncLib->resolve( #function );	\
	if( !m_ ## function ) {												\
		ResolvingFailed();												\
		return false;													\
	}

	RESOLVE ( GetSpringVersion );
	RESOLVE ( GetSpringVersionPatchset );

	RESOLVE ( Init );
	RESOLVE ( UnInit );

	RESOLVE ( GetWritableDataDirectory );
	RESOLVE ( GetDataDirectoryCount );
	RESOLVE ( GetDataDirectory );

	RESOLVE ( ProcessUnits );
	RESOLVE ( ProcessUnitsNoChecksum );
	RESOLVE ( GetUnitCount );
	RESOLVE ( GetUnitName );
	RESOLVE ( GetFullUnitName );

	RESOLVE ( AddArchive );
	RESOLVE ( AddAllArchives );
	RESOLVE ( RemoveAllArchives );
	RESOLVE ( GetArchiveChecksum );
	RESOLVE ( GetArchivePath );

	RESOLVE ( GetMapCount );
	RESOLVE ( GetMapName );
	RESOLVE ( GetMapDescription );
	RESOLVE ( GetMapAuthor );
	RESOLVE ( GetMapWidth );
	RESOLVE ( GetMapHeight );
	RESOLVE ( GetMapTidalStrength );
	RESOLVE ( GetMapWindMin );
	RESOLVE ( GetMapWindMax );
	RESOLVE ( GetMapGravity );
	RESOLVE ( GetMapResourceCount );
	RESOLVE ( GetMapResourceName );
	RESOLVE ( GetMapResourceMax );
	RESOLVE ( GetMapResourceExtractorRadius );
	RESOLVE ( GetMapPosCount );
	RESOLVE ( GetMapPosX );
	RESOLVE ( GetMapPosZ );
//	RESOLVE ( GetMapInfoEx );
//	RESOLVE ( GetMapInfo );
	RESOLVE ( GetMapArchiveCount );
	RESOLVE ( GetMapArchiveName );
	RESOLVE ( GetMapChecksum );
	RESOLVE ( GetMapChecksumFromName );
	RESOLVE ( GetMinimap );
	RESOLVE ( GetInfoMapSize );
	RESOLVE ( GetInfoMap );
	RESOLVE ( GetMapMinHeight );
	RESOLVE ( GetMapMaxHeight );

	RESOLVE ( GetSkirmishAICount );
	RESOLVE ( GetSkirmishAIInfoCount );
	RESOLVE ( GetInfoKey );
	RESOLVE ( GetInfoType );
	RESOLVE ( GetInfoValueString );
	RESOLVE ( GetInfoValueInteger );
	RESOLVE ( GetInfoValueFloat );
	RESOLVE ( GetInfoValueBool );
	RESOLVE ( GetInfoDescription );

	RESOLVE ( GetPrimaryModCount );
	RESOLVE ( GetPrimaryModInfoCount );
//	RESOLVE ( GetPrimaryModName );
//	RESOLVE ( GetPrimaryModShortName );
//	RESOLVE ( GetPrimaryModVersion );
//	RESOLVE ( GetPrimaryModMutator );
//	RESOLVE ( GetPrimaryModGame );
//	RESOLVE ( GetPrimaryModShortGame );
//	RESOLVE ( GetPrimaryModDescription );
	RESOLVE ( GetPrimaryModArchive );
	RESOLVE ( GetPrimaryModArchiveCount );
	RESOLVE ( GetPrimaryModArchiveList );
	RESOLVE ( GetPrimaryModIndex );
	RESOLVE ( GetPrimaryModChecksum );
	RESOLVE ( GetPrimaryModChecksumFromName );
	RESOLVE ( GetSideCount );
	RESOLVE ( GetSideName );

	RESOLVE ( GetMapOptionCount );
	RESOLVE ( GetModOptionCount );
	RESOLVE ( GetOptionKey );
	RESOLVE ( GetOptionName );
	RESOLVE ( GetOptionSection );
	RESOLVE ( GetOptionStyle );
	RESOLVE ( GetOptionDesc );
	RESOLVE ( GetOptionType );
	RESOLVE ( GetOptionBoolDef );
	RESOLVE ( GetOptionNumberDef );
	RESOLVE ( GetOptionNumberMin );
	RESOLVE ( GetOptionNumberMax );
	RESOLVE ( GetOptionNumberStep );
	RESOLVE ( GetOptionStringDef );
	RESOLVE ( GetOptionStringMaxLen );
	RESOLVE ( GetOptionListCount );
	RESOLVE ( GetOptionListDef );
	RESOLVE ( GetOptionListItemKey );
	RESOLVE ( GetOptionListItemName );
	RESOLVE ( GetOptionListItemDesc );

	RESOLVE ( GetModValidMapCount );
	RESOLVE ( GetModValidMap );

	RESOLVE ( OpenFileVFS );
	RESOLVE ( CloseFileVFS );
	RESOLVE ( ReadFileVFS );
	RESOLVE ( FileSizeVFS );
	RESOLVE ( InitFindVFS );
	RESOLVE ( InitDirListVFS );
	RESOLVE ( InitSubDirsVFS );
	RESOLVE ( FindFilesVFS );

	RESOLVE ( OpenArchive );
	RESOLVE ( CloseArchive );
	RESOLVE ( FindFilesArchive );
	RESOLVE ( OpenArchiveFile );
	RESOLVE ( ReadArchiveFile );
	RESOLVE ( CloseArchiveFile );
	RESOLVE ( SizeArchiveFile );

	RESOLVE ( SetSpringConfigFile )
	RESOLVE ( GetSpringConfigFile )
	RESOLVE ( GetSpringConfigString );
	RESOLVE ( GetSpringConfigInt );
	RESOLVE ( GetSpringConfigFloat );
	RESOLVE ( SetSpringConfigString );
	RESOLVE ( SetSpringConfigInt );
	RESOLVE ( SetSpringConfigFloat );

#ifdef Q_OS_WIN
	// Change current dir back to default
	QDir::setCurrent( originalDir );
#endif
	return true;
}

bool UnitSyncLibApi::UnloadLibrary()
{
	return _unitSyncLib->unload();
}

bool UnitSyncLibApi::Initialize()
{
	int success = m_Init( 0, 0 );
	DEBUG_PRINT2( "UNITSYNC :: Init(0,0) = %i", success );
	if( !success )
	{
		qWarning( "UNITSYNC :: Init ERROR" );

		_unitSyncLib->unload();

		return false;
	}

	success = CheckUnitSyncVersion();
	if( !success )
	{
		_unitSyncLib->unload();

		return false;
	}

#ifdef UNITSYNC_DEBUG
	qDebug( "UNITSYNC :: GetSpringConfigFile() = %s", m_GetSpringConfigFile() );
	int dirCount = m_GetDataDirectoryCount();
	qDebug ( "UNITSYNC :: GetDataDirectoryCount() = %i dirs", dirCount );

	for( int i = 0; i < dirCount; ++i )
		qDebug ( "UNITSYNC :: GetDataDirectory() = %s", m_GetDataDirectory( i ) );

	qDebug( "UNITSYNC :: GetWritableDataDirectory() = %s", m_GetWritableDataDirectory() );
#endif

	return true;
}

bool UnitSyncLibApi::IsLoaded( bool isWarn ) const
{
	if( !_unitSyncLib->isLoaded() ) {
		if( isWarn )
			qWarning( "UNITSYNC :: Library is not loaded, you can't use it!" );

		return false;
	}

	return true;
}

void UnitSyncLibApi::ResolvingFailed()
{
	qWarning( "UNITSYNC :: Resolving FAIL: %s", qPrintable( _unitSyncLib->errorString() ) );
	_unitSyncLib->unload();
}

#define MIN_UNITSYNC_VERSION 91
#define MAX_UNITSYNC_VERSION 100

int UnitSyncLibApi::CheckUnitSyncVersion()
{
	QString springVersion = m_GetSpringVersion();
	int major = springVersion.section( '.', 0, 0 ).toInt();

	if( major < MIN_UNITSYNC_VERSION )
	{
		qWarning( "UNITSYNC :: can't use old library. Current library version %s, "
				  "you need at least %i",
				  qPrintable( springVersion ), MIN_UNITSYNC_VERSION );
		return 0;
	}

	if( major > MAX_UNITSYNC_VERSION )
	{
		qWarning( "UNITSYNC :: can't use new library. Current library %s, "
				  "maximum supported version %i",
				  qPrintable( springVersion ), MAX_UNITSYNC_VERSION );
		return 0;
	}

	return 1;
}
