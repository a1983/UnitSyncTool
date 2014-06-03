#ifndef REQUESTPROCESSOR_H
#define REQUESTPROCESSOR_H

#include <QByteArray>
#include <QMap>

#include "ResponseType.h"

class RequestProcessor
{
public:
	RequestProcessor();
	virtual ~RequestProcessor();

	bool ProcessRequest( const QByteArray& data );

	void			SetResponse	( const QByteArray& data, ResponseType type );
	QByteArray		Response	() const;
	ResponseType	Type		() const;

private:
	virtual bool BuildResponse		() = 0;
	virtual bool BuildSubResponse	( const QByteArray& subRequest, const QByteArray& data ) = 0;

private:
	QByteArray		_responseData;
	ResponseType	_responseType;
};

#endif // REQUESTPROCESSOR_H
