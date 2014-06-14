#include "UnitSyncFileManager.h"

#include <QDir>
#include <QFile>

#ifdef WIN32
const QString _unitSyncFile = "unitsync.dll";
#else
const QString _unitSyncFile = "libunitsync.so";
#endif
const QString _springSettingsFile = "springsettings.cfg";

UnitSyncFileManager::UnitSyncFileManager()
{

}

void UnitSyncFileManager::SetDataDirectory( const QString& springDataDirectory , UnitSyncFileManagerMode mode )
{
	_springDataDirectory = springDataDirectory;
	_mode = mode;
	if( _mode == UnitSync_MultiEngine )
		EnumEngines();
	else {
		_enginePaths.clear();
		_selectedEngine.clear();
	}
}

QString UnitSyncFileManager::DataDirectory() const
{
	return QDir::toNativeSeparators( _springDataDirectory );
}

QString UnitSyncFileManager::UnitSyncFile() const
{
	if( _mode == UnitSync_MultiEngine )
		return SelectedEnginePath().append( "/" ).append( _unitSyncFile );
	else
		return DataDirectory().append( "/" ).append( _unitSyncFile );
}

QString UnitSyncFileManager::SpringConfigFile() const
{
	return _springDataDirectory + _springSettingsFile;
}

void UnitSyncFileManager::SelectEngine( const QString& engine )
{
	_selectedEngine = engine;
	qDebug( "Engine selected %s", qPrintable( _selectedEngine ) );
}

UnitSyncFileManagerMode UnitSyncFileManager::Mode() const
{
	return _mode;
}

void UnitSyncFileManager::EnumEngines()
{
	_enginePaths.clear();
	_selectedEngine.clear();

	QDir enginesDirectory( _springDataDirectory + "/engines" );
	QStringList engines = enginesDirectory.entryList( QDir::Dirs | QDir::NoDotAndDotDot );

	foreach( const QString& engine, engines ) {
		if( enginesDirectory.exists( engine + "/" + _unitSyncFile ) ) {
			_enginePaths.insert( engine, enginesDirectory.absoluteFilePath( engine ) );
			qDebug( "Engine found %s", qPrintable( engine ) );
		}
	}

	if( _enginePaths.isEmpty() ) {
		qDebug( "No engines found in %s", qPrintable( _springDataDirectory + "/engines" ) );
		return;
	}

	SelectEngine( _enginePaths.keys().last() );
}

QString UnitSyncFileManager::SelectedEnginePath() const
{
	return _enginePaths.value( _selectedEngine );
}
