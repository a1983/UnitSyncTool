#include "CommandLineParser.h"

#include <QDebug>
#include <QDir>
#include <QRegExp>
#include <QStringList>

quint16 _defaultPort = 8668;

QString _unitsyncDir    = "unitsyncDir";
QString _springDataDir	= "springDataDir";
QString _serverPort		= "port";

CommandLineParser::CommandLineParser( const QStringList& arguments )
{
	QStringList commandList = arguments.filter( QRegExp( "^--" ) );
	foreach( const QString& command, commandList ) {
		int splitterPos = command.indexOf( '=' );
		if( splitterPos < 0 ) {
			_arguments.insert( command.mid( 2 ), QString() );
		}
		else {
			QString key = command.mid( 2, splitterPos - 2 );
			QString val = command.mid( splitterPos + 1 );
			_arguments.insert( key, val );
		}
	}
}

QString CommandLineParser::UnitSyncDir() const
{
	QString unitSyncDir = QDir::cleanPath( Value( _unitsyncDir ) );
	if( unitSyncDir.isEmpty() )
		return ".";
	else return unitSyncDir;
}

QString CommandLineParser::SpringDataDir() const
{
	return QDir::cleanPath( Value( _springDataDir ) );
}

quint16 CommandLineParser::Port() const
{
	int value = Value( _serverPort ).toInt();
	if( !value )
		return _defaultPort;
	return value;
}

bool CommandLineParser::IsExist( const QString& argument ) const
{
	return _arguments.contains( argument );
}

QString CommandLineParser::Value( const QString& argument ) const
{
	return _arguments.value( argument );
}
