#ifndef UNITSYNC_MAPS_PROCESSOR_H
#define UNITSYNC_MAPS_PROCESSOR_H

#include "RequestProcessor.h"

class UnitSyncMapsProcessor : public RequestProcessor
{
public:
	UnitSyncMapsProcessor();

	// RequestProcessor interface
private:
	virtual bool BuildResponse();
	virtual bool BuildSubResponse( const QByteArray& subRequest, const QByteArray& data );

private:
	bool ProcessInfoRequest		( const QByteArray& map );
	bool ProcessTerrainRequest	( const QByteArray& map );
	bool ProcessHeightRequest	( const QByteArray& map );
	bool ProcessMetalRequest	( const QByteArray& map );
	bool ProcessOptionsRequest	( const QByteArray& map, const QList<QByteArray>& subRequest );
};

#endif // UNITSYNC_MAPS_PROCESSOR_H
