#ifndef MAPINFO_H
#define MAPINFO_H

#include <QString>
#include <QPoint>
#include <QList>

enum MipLevelEnum
{
	mip1024x1024 = 0,
	mip512x512
};

struct MapInfo
{
	QString Author;			// max 200 chars
	QString Description;	// max 255 chars

	int		TidalStrength;
	int		Gravity;

	float	MaxMetal;
	int		ExtractorRadius;

	int		MinWind;
	int		MaxWind;

	int		Width;
	int		Height;

	QList< QPoint > StartPositions; // Max 16
};

#endif // MAPINFO_H
