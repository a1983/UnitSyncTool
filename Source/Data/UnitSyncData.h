#ifndef UNITSYNCDATA_H
#define UNITSYNCDATA_H

#include <QStringList>
#include <QHash>
#include <QSet>
#include <QMetaType>

#include "SkirmishAiInfo.h"
#include "SideInfo.h"
#include "SpringOption.h"

enum UnitSyncFileManagerMode {
	UnitSync_MultiEngine,
	UnitSync_SingleEngine,
};

struct UnitSyncData
{
	QString _springVersion;
	QString _springVersionPatchset;

	QHash< QString, uint >					_modChecksumByName;
	QHash< QString, QList< SpringOption > >	_modOptionsByName;

	QHash< QString,  int >					_mapIndexByName;
	QHash< QString, uint >					_mapChecksumByName;
	QHash< QString, QList< SpringOption > >	_mapOptionsByName;

	QString		_currentModName;
//	QStringList _currentModDependcies;

	QSet < QString >		_validMaps;
	QList< SkirmishAiInfo >	_skirmishAiInfo;
	QList< SideInfo >		_sideInfoCache;

	enum ClearModeEnum { OnlyGame, All };

	void Clear( int clearMode = All )
	{
		if( clearMode == All )
		{
			_springVersion.clear();
			_springVersion.clear();

			_modChecksumByName.clear();
			_modOptionsByName.clear();

			_mapIndexByName.clear();
			_mapChecksumByName.clear();
			_mapOptionsByName.clear();

		}

		_currentModName.clear();
//		_currentModDependcies.clear();

		_validMaps.clear();
		_skirmishAiInfo.clear();
		_sideInfoCache.clear();
	}
};

Q_DECLARE_METATYPE( UnitSyncData )

#endif // UNITSYNCDATA_H
