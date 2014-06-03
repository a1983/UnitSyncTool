#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QString>
#include <QMap>

class CommandLineParser
{
public:
	CommandLineParser( const QStringList& arguments );

	QString UnitSyncDir() const;
	QString SpringDataDir() const;
	quint16 Port() const;

private:
	bool	IsExist	( const QString& argument ) const;
	QString Value	( const QString& argument ) const;

private:
	QMap< QString, QString > _arguments;
};

#endif // COMMANDLINEPARSER_H
