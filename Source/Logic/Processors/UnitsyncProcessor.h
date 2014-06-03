#ifndef UNITSYNC_PROCESSOR_H
#define UNITSYNC_PROCESSOR_H

#include <QMap>

#include "RequestProcessor.h"

class UnitSyncProcessor : public RequestProcessor
{
public:
	UnitSyncProcessor();
	~UnitSyncProcessor();

	// RequestProcessor interface
private:
	virtual bool BuildResponse();
	virtual bool BuildSubResponse( const QByteArray& subRequest, const QByteArray& data );

private:
	void AppendSubProcessor( const QByteArray& name, RequestProcessor* processor );
	void AppendHtmlLink( const QByteArray& name );
	void AppendSubProcessorToMap( const QByteArray& name, RequestProcessor* processor );

private:
	QByteArray								_html;
	QMap< QByteArray, RequestProcessor* >	_processors;
};

#endif // UNITSYNC_PROCESSOR_H
