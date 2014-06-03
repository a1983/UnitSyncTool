#ifndef UNITSYNC_GAMES_LIST_PROCESSOR_H
#define UNITSYNC_GAMES_LIST_PROCESSOR_H

#include "RequestProcessor.h"

class UnitSyncGamesListProcessor : public RequestProcessor
{
public:
	UnitSyncGamesListProcessor();

	// RequestProcessor interface
private:
	virtual bool BuildResponse();
	virtual bool BuildSubResponse( const QByteArray& subRequest, const QByteArray& data );
};

#endif // UNITSYNC_GAMES_LIST_PROCESSOR_H
