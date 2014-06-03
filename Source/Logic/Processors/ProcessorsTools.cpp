#include "ProcessorsTools.h"

#include <QBuffer>
#include <QImage>

QByteArray ProcessorsTools::BuildMenu(const QByteArray& prefix, const QList< QByteArray >& items ) {
	QByteArray result;

	QByteArray suffix = "\">";
	QByteArray end = "</a><br>";

	for( int i = 0; i < items.size(); ++i ) {
		const QByteArray& item = items[ i ];
		result.append( prefix ).append( item ).append( suffix ).append( item ).append( end );
	}

	return result;
}

QByteArray ProcessorsTools::BuildOptionsList( const QList< SpringOption >& options )
{
	QByteArray response;
	for( int i = 0; i < options.size(); ++i ) {
		const SpringOption& option = options[ i ];
		response.append( option[ SOI_KEY ].toString().toUtf8() ).append( '\n' );
	}
	return response;
}


QByteArray ProcessorsTools::BuildOption( const QByteArray& key, const QList<SpringOption>& items ) {
	QByteArray result;
	int index = -1;
	for( int i = 0; i < items.size(); ++i ) {
		if( items[ i ][ SOI_KEY ].toByteArray() == key ) {
			index = i;
			break;
		}
	}

	if( index < 0 )
		return result;

	const SpringOption& info = items[ index ]; {
		QMapIterator< SpringOptionIndex, QVariant > i( info );
		while( i.hasNext() ) {
			i.next();
			if( i.key() == SOI_LIST ) {
				QVariantList list = i.value().toList();
				foreach( const QVariant& listItem, list ) {
					QVariantList item = listItem.toList();
					foreach( const QVariant& field, item ) {
						result.append( '\t' ).append( ReplaceNewLine( field.toString() ) ).append( '\n' );
					}
				}
			}
			else
				result.append( ReplaceNewLine( i.value().toString() ) ).append( '\n' );
		}
	}

	return result;
}


QByteArray ProcessorsTools::FromImage( const QImage& image )
{
	QByteArray result;

	QBuffer buffer( &result );
	buffer.open( QBuffer::WriteOnly );
	image.save( &buffer, "png" );

	return result;
}


QByteArray ProcessorsTools::ReplaceNewLine( const QString& text )
{
	QByteArray withoutNL( text.toUtf8() );
	return withoutNL.replace( '\n', "\\n" );
}
