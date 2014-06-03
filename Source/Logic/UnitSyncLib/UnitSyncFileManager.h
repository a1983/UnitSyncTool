#ifndef UNITSYNCFILEMANAGER_H
#define UNITSYNCFILEMANAGER_H

#include <QMap>

#include "Data/UnitSyncData.h"

class UnitSyncFileManager
{
public:
	UnitSyncFileManager();

	void SetDataDirectory( const QString& springDataDirectory, UnitSyncFileManagerMode mode );
	QString DataDirectory() const;

	QString UnitSyncFile() const;
	QString SpringConfigFile() const;

	void SelectEngine( const QString& engine );

	UnitSyncFileManagerMode Mode() const;

private:
	void EnumEngines();
	QString SelectedEnginePath() const;

private:
	QString						_springDataDirectory;

	QMap< QString, QString >	_enginePaths;
	QString						_selectedEngine;

	UnitSyncFileManagerMode		_mode;
};

#endif // UNITSYNCFILEMANAGER_H
