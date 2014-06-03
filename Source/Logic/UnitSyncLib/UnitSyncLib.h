#ifndef UNITSYNCLIB_H
#define UNITSYNCLIB_H

#include "Data/MapInfo.h"
#include "Data/SpringOption.h"
#include "Data/UnitSyncData.h"

#include "UnitSyncLibApi.h"
#include "UnitSyncFileManager.h"

class UnitSyncLib :
		public UnitSyncLibApi
{
private:
	// Singleton access
	friend UnitSyncLib* UnitSync();
	UnitSyncLib();
	~UnitSyncLib();

public:
	// Set Path ------------------------------------------------------------------------------------
	void SetSpringDataDirectory	( const QString& springDataDirectory );
	void SetUnitSyncDirectory	( const QString& springDataDirectory );

	// Main info functions -------------------------------------------------------------------------
	QString SpringVersionFull();
	QString SpringVersion();

	// Map functions -------------------------------------------------------------------------------
	QList< QString >		GetMapsList		();
	uint					GetMapHash		( const QString& mapName );
	QList< SpringOption >	GetMapOptions	( const QString& mapName );
	MapInfo					GetMinimapInfo	( const QString& mapName );
	QImage					GetTerrain		( const QString& mapName, int mapX, int mapY, MipLevelEnum mipLevel = mip512x512 );
	QImage					GetHeightMap	( const QString& mapName, MipLevelEnum mipLevel = mip512x512 );
	QImage					GetMetalMap		( const QString& mapName, const QImage& terrain );

	// Game functions ------------------------------------------------------------------------------
	QList< QString >		GetGamesList	();
	uint					GetGameHash		( const QString& gameName );
	QList< SideInfo >		GetGameSides	( const QString& gameName );
	QList< SkirmishAiInfo >	GetGameAIList	( const QString& gameName );
	QList< QString >		GetGameValidMaps( const QString& gameName );
	QList< SpringOption >	GetGameOptions	( const QString& gameName );

private:
	// Helpers -------------------------------------------------------------------------------------
	bool CheckUnitSyncLoaded	();
	bool CheckGameArchiveLoaded	( const QString& gameName );

	bool Load					();
	bool Reload					();

	bool SetCurrentGame			( const QString& gameName );

	void SetupDataDirectory		();

	void InitSpringVersion		();
	void InitGames				();
	void InitMaps				();

	void InitSkirmishAICache	();
	void InitValidMapsCache		();
	void InitSideCache			();

	QImage						GetSideIcon	( const QString& sideName );
	QByteArray					GetVFSFile	( const QString& fileName );
	QHash< QString, QString >	BuildInfo	( int infoItemCount );

	QList< SpringOption >		GetOptionList	( int count );
	SpringOption				GetOption		( int index );

private:
	UnitSyncData			_data;
	UnitSyncFileManager		_fileManager;
};

// Singleton access
UnitSyncLib* UnitSync();

#endif
