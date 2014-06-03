#ifndef UNITSYNCLIBAPI_H
#define UNITSYNCLIBAPI_H

#include <QLibrary>

// https://github.com/spring/spring/blob/master/tools/unitsync/unitsync_api.h

#ifdef Q_OS_WIN
#define CONV __stdcall
#else
#define CONV
#endif

#define UNITSYNC_DEBUG

#ifdef UNITSYNC_DEBUG
#define DEBUG_PRINT(s) \
	qDebug(s);
#define DEBUG_PRINT2(s, s2) \
	qDebug(s, s2);
#define DEBUG_PRINT3(s, s2, s3) \
	qDebug(s, s2, s3);
#else
#define DEBUG_PRINT(s)
#define DEBUG_PRINT2(s, s2)
#define DEBUG_PRINT3(s, s2, s3)
#endif

class UnitSyncLibApi
{
protected:
	UnitSyncLibApi();
	~UnitSyncLibApi();

public:
	bool IsLoaded( bool isWarn = true ) const;

protected:
	bool LoadLibrary( const QString& libraryFile );
	bool UnloadLibrary();

	bool Initialize();

protected:
#define DECLARE_IMPORT( return_signature, function, signature ) \
	typedef return_signature ( CONV * function ) signature; \
	function m_ ## function;

	DECLARE_IMPORT ( const char*, GetSpringVersion, 		() )
	DECLARE_IMPORT ( const char*, GetSpringVersionPatchset, () )

	DECLARE_IMPORT ( int		, Init, 	( bool isServer, int id ) )
	DECLARE_IMPORT ( void		, UnInit,	() )

	DECLARE_IMPORT ( const char*, GetWritableDataDirectory,	() )
	DECLARE_IMPORT ( int		, GetDataDirectoryCount,	() )
	DECLARE_IMPORT ( const char*, GetDataDirectory,			( int index ) )

	DECLARE_IMPORT ( int		, ProcessUnits,				( void ) )
	DECLARE_IMPORT ( int		, ProcessUnitsNoChecksum,	( void ) )
	DECLARE_IMPORT ( int		, GetUnitCount,				() )
	DECLARE_IMPORT ( const char*, GetUnitName,				( int unit ) )
	DECLARE_IMPORT ( const char*, GetFullUnitName,			( int unit ) )

	DECLARE_IMPORT ( void		, AddArchive,			( const char* name ) )
	DECLARE_IMPORT ( void		, AddAllArchives,		( const char* root ) )
	DECLARE_IMPORT ( void		, RemoveAllArchives,	() )
	DECLARE_IMPORT ( uint		, GetArchiveChecksum,	( const char* arname ) )
	DECLARE_IMPORT ( const char*, GetArchivePath,		( const char* arname ) )

	DECLARE_IMPORT ( int		, GetMapCount,			() )
	DECLARE_IMPORT ( const char*, GetMapName,			( int index ) )
	DECLARE_IMPORT ( const char*, GetMapDescription,	( int index ) )
	DECLARE_IMPORT ( const char*, GetMapAuthor,			( int index ) )
	DECLARE_IMPORT ( int		, GetMapWidth,			( int index ) )
	DECLARE_IMPORT ( int		, GetMapHeight,			( int index ) )
	DECLARE_IMPORT ( int		, GetMapTidalStrength,	( int index ) )
	DECLARE_IMPORT ( int		, GetMapWindMin,		( int index ) )
	DECLARE_IMPORT ( int		, GetMapWindMax,		( int index ) )
	DECLARE_IMPORT ( int		, GetMapGravity,		( int index ) )
	DECLARE_IMPORT ( int		, GetMapResourceCount,	( int index ) )
	DECLARE_IMPORT ( const char*, GetMapResourceName,	( int index, int resourceIndex ) )
	DECLARE_IMPORT ( float		, GetMapResourceMax,	( int index, int resourceIndex ) )
	DECLARE_IMPORT ( int		, GetMapResourceExtractorRadius,	( int index, int resourceIndex ) )
	DECLARE_IMPORT ( int		, GetMapPosCount,		( int index ) )
	DECLARE_IMPORT ( float		, GetMapPosX,			( int index, int posIndex ) )
	DECLARE_IMPORT ( float 		, GetMapPosZ,			( int index, int posIndex ) )
//	DECLARE_IMPORT ( int		, GetMapInfoEx,			( const char* name, MapInfoApi* outInfo, int version ) )
//	DECLARE_IMPORT ( int		, GetMapInfo,			( const char* name, MapInfoApi* outInfo ) )
	DECLARE_IMPORT ( uint		, GetMapChecksum,		( int index ) )
	DECLARE_IMPORT ( uint		, GetMapChecksumFromName, ( const char* mapName ) )
	DECLARE_IMPORT ( int		, GetMapArchiveCount,	( const char* mapName ) )
	DECLARE_IMPORT ( const char*, GetMapArchiveName,	( int archiveIndex ) )
	DECLARE_IMPORT ( ushort*	, GetMinimap,			( const char* filename, int miplevel ) )
	DECLARE_IMPORT ( int		, GetInfoMapSize,		( const char* filename, const char* name, int* width, int* height ) )
	DECLARE_IMPORT ( int		, GetInfoMap,			( const char* filename, const char* name, void* data, int typeHint ) )
	DECLARE_IMPORT ( float		, GetMapMinHeight,		( const char* name) )
	DECLARE_IMPORT ( float		, GetMapMaxHeight,		( const char* name) )

	DECLARE_IMPORT ( int		, GetSkirmishAICount,		() )
	DECLARE_IMPORT ( int		, GetSkirmishAIInfoCount,	( int index ) )
	DECLARE_IMPORT ( const char*, GetInfoKey,				( int index ) )
	DECLARE_IMPORT ( const char*, GetInfoType,				( int index ) )
	DECLARE_IMPORT ( const char*, GetInfoValueString,		( int index ) )
	DECLARE_IMPORT ( int		, GetInfoValueInteger,		( int index ) )
	DECLARE_IMPORT ( float		, GetInfoValueFloat,		( int index ) )
	DECLARE_IMPORT ( bool		, GetInfoValueBool,			( int index ) )
	DECLARE_IMPORT ( const char*, GetInfoDescription,		( int index ) )

	DECLARE_IMPORT ( int		, GetPrimaryModCount,		() )
	DECLARE_IMPORT ( int		, GetPrimaryModInfoCount,	( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModName,		( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModShortName,	( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModVersion,		( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModMutator,		( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModGame,		( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModShortGame,	( int index ) )
//	DECLARE_IMPORT ( const char*, GetPrimaryModDescription,	( int index ) )
	DECLARE_IMPORT ( const char*, GetPrimaryModArchive,		( int index ) )
	DECLARE_IMPORT ( int		, GetPrimaryModArchiveCount,( int index ) )
	DECLARE_IMPORT ( const char*, GetPrimaryModArchiveList,	( int arnr ) )
	DECLARE_IMPORT ( int		, GetPrimaryModIndex,		( const char* name ) )
	DECLARE_IMPORT ( uint		, GetPrimaryModChecksum,	( int index ) )
	DECLARE_IMPORT ( uint		, GetPrimaryModChecksumFromName, ( const char* name ) )

	DECLARE_IMPORT ( int		, GetSideCount,				() )
	DECLARE_IMPORT ( const char*, GetSideName,				( int side ) )

	DECLARE_IMPORT ( int		, GetSkirmishAIOptionCount,	( int index ) )
	DECLARE_IMPORT ( int		, GetMapOptionCount,		( const char* name ) )
	DECLARE_IMPORT ( int		, GetModOptionCount,		() )
	DECLARE_IMPORT ( const char*, GetOptionKey,				( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionName,			( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionSection,			( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionStyle,			( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionDesc,			( int optIndex ) )
	DECLARE_IMPORT ( int		, GetOptionType,			( int optIndex ) )
	DECLARE_IMPORT ( int		, GetOptionBoolDef,			( int optIndex ) )
	DECLARE_IMPORT ( float		, GetOptionNumberDef,		( int optIndex ) )
	DECLARE_IMPORT ( float		, GetOptionNumberMin,		( int optIndex ) )
	DECLARE_IMPORT ( float		, GetOptionNumberMax,		( int optIndex ) )
	DECLARE_IMPORT ( float		, GetOptionNumberStep,		( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionStringDef,		( int optIndex ) )
	DECLARE_IMPORT ( int		, GetOptionStringMaxLen,	( int optIndex ) )
	DECLARE_IMPORT ( int		, GetOptionListCount,		( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionListDef,			( int optIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionListItemKey,		( int optIndex, int itemIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionListItemName,	( int optIndex, int itemIndex ) )
	DECLARE_IMPORT ( const char*, GetOptionListItemDesc,	( int optIndex, int itemIndex ) )

	DECLARE_IMPORT ( int		, GetModValidMapCount,		() )
	DECLARE_IMPORT ( const char*, GetModValidMap,			( int index ) )

	DECLARE_IMPORT ( int		, OpenFileVFS,		( const char* name ) )
	DECLARE_IMPORT ( void		, CloseFileVFS,		( int handle ) )
	DECLARE_IMPORT ( void		, ReadFileVFS,		( int handle, void* buf, int length ) )
	DECLARE_IMPORT ( int		, FileSizeVFS,		( int handle ) )
	DECLARE_IMPORT ( int		, InitFindVFS,		( const char* pattern ) )
	DECLARE_IMPORT ( int        , InitDirListVFS,	( const char* path, const char* pattern, const char* modes ) )
	DECLARE_IMPORT ( int		, InitSubDirsVFS,	( const char* path, const char* pattern, const char* modes ) )
	DECLARE_IMPORT ( int		, FindFilesVFS,		( int handle, char* nameBuf, int size ) )

	DECLARE_IMPORT ( int		, OpenArchive,		( const char* name ) )
	DECLARE_IMPORT ( void		, CloseArchive,		( int archive ) )
	DECLARE_IMPORT ( int		, FindFilesArchive,	( int archive, int cur, char* nameBuf, int* size ) )
	DECLARE_IMPORT ( int		, OpenArchiveFile,	( int archive, const char* name ) )
	DECLARE_IMPORT ( int		, ReadArchiveFile,	( int archive, int handle, void* buffer, int numBytes ) )
	DECLARE_IMPORT ( void		, CloseArchiveFile,	( int archive, int handle ) )
	DECLARE_IMPORT ( int		, SizeArchiveFile,	( int archive, int handle ) )

	DECLARE_IMPORT ( void		, SetSpringConfigFile,	( const char* fileNameAsAbsolutePath ) )
	DECLARE_IMPORT ( const char*, GetSpringConfigFile,	() )
	DECLARE_IMPORT ( const char*, GetSpringConfigString,( const char* name, const char* defvalue ) )
	DECLARE_IMPORT ( void		, SetSpringConfigString,( const char* name, const char* value ) )
	DECLARE_IMPORT ( int		, GetSpringConfigInt,	( const char* name, const int defvalue ) )
	DECLARE_IMPORT ( void		, SetSpringConfigInt,	( const char* name, const int value ) )
	DECLARE_IMPORT ( float		, GetSpringConfigFloat,	( const char* name, const float defvalue ) )
	DECLARE_IMPORT ( void		, SetSpringConfigFloat,	( const char* name, const float value ) )

private:
	void ResolvingFailed();
	int CheckUnitSyncVersion();

private:
	QLibrary* _unitSyncLib;
};

#endif // UNITSYNCLIBAPI_H
