#include <QCoreApplication>

#include "Logic/UnitSyncLib/UnitSyncLib.h"
#include "Logic/ServerManager.h"
#include "Logic/Processors/UnitSyncProcessor.h"
#include "Logic/CommandLineParser.h"

int main( int argc, char* argv [] )
{
	QCoreApplication application( argc, argv );

	CommandLineParser commands( application.arguments() );

	const QString dataPath = commands.SpringDataDir();
	if( !dataPath.isEmpty() ) {
		qDebug( "Tool works in multi engine mode.\nData directory, '%s'.", qPrintable( dataPath ) );
		UnitSync()->SetSpringDataDirectory( dataPath );
	}
	else {
		const QString unitSyncPath = commands.UnitSyncDir();
		if( unitSyncPath == "." )
			qDebug( "Tool works in normal mode.\nUnitSync works in current directory." );
		else
			qDebug( "Tool works in normal mode.\nUnitSync directory, '%s'.", qPrintable( unitSyncPath ) );

		UnitSync()->SetUnitSyncDirectory( unitSyncPath );
	}

	quint16 port = commands.Port();

	UnitSyncProcessor requestProcessor;

	ServerManager server( &requestProcessor );

	qDebug( "Server try to listen local port, '%i'.", port );
	if( !server.Initialize( port ) ) {
		qWarning( "Server not runned!" );
		return -1;
	}

	qDebug( "Server ready." );

	return application.exec();
}
