#ifndef PROCESSORSTOOLS_H
#define PROCESSORSTOOLS_H

#include <QByteArray>
#include <QList>

#include "Data/SpringOption.h"

namespace ProcessorsTools
{
QByteArray BuildMenu		( const QByteArray& prefix, const QList< QByteArray >& items );
QByteArray BuildOptionsList	( const QList< SpringOption >& options );
QByteArray BuildOption		( const QByteArray& key, const QList<SpringOption>& items );
QByteArray FromImage		( const QImage& image );
QByteArray ReplaceNewLine	( const QString& text );
}

#endif // PROCESSORSTOOLS_H
