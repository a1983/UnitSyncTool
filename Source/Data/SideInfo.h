#ifndef SIDEINFO_H
#define SIDEINFO_H

#include <QString>
#include <QImage>

struct SideInfo
{
	SideInfo( const QString& name = QString(), const QImage& icon = QImage() ) :
		Icon( icon ),
		Name( name )
	{
	}

	QImage Icon;
	QString Name;

};

#endif // SIDEINFO_H
