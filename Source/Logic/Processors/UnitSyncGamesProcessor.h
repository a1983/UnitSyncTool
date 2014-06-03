#ifndef UNITSYNCGAMESPROCESSOR_H
#define UNITSYNCGAMESPROCESSOR_H

#include "RequestProcessor.h"

class UnitSyncGamesProcessor : public RequestProcessor
{
public:
	UnitSyncGamesProcessor();

	// RequestProcessor interface
private:
	virtual bool BuildResponse();
	virtual bool BuildSubResponse( const QByteArray& subRequest, const QByteArray& data );

private:
	bool ProcessSideRequest			( const QByteArray& game, const QList< QByteArray >& subRequest );
	bool ProcessAIListRequest		( const QByteArray& game, const QList< QByteArray >& subRequest );
	bool ProcessValidMapsRequest	( const QByteArray& game );
	bool ProcessOptionsRequest		( const QByteArray& game, const QList< QByteArray >& subRequest );
};

#endif // UNITSYNCGAMESPROCESSOR_H
