#ifndef UNITSYNC_MAPS_LIST_PROCESSOR_H
#define UNITSYNC_MAPS_LIST_PROCESSOR_H

#include "RequestProcessor.h"

class UnitSyncMapsListProcessor : public RequestProcessor
{
public:
	UnitSyncMapsListProcessor();

	// RequestProcessor interface
private:
	virtual bool BuildResponse();
	virtual bool BuildSubResponse( const QByteArray& subRequest, const QByteArray& data );
};

#endif // UNITSYNC_MAPS_LIST_PROCESSOR_H
