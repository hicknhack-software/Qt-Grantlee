/*
  This file is part of the Grantlee template system.

  Copyright (c) 2010 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version
  2.1 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "nulllocalizer_p.h"

#include <QtCore/QDateTime>

using namespace Grantlee;

NullLocalizer::NullLocalizer()
{

}

NullLocalizer::~NullLocalizer()
{

}

QString NullLocalizer::localizeDate( const QDate &date, QLocale::FormatType formatType ) const
{
  Q_UNUSED( formatType )
  return date.toString();
}

QString NullLocalizer::localizeTime( const QTime& time, QLocale::FormatType formatType ) const
{
  Q_UNUSED( formatType )
  return time.toString();
}

QString NullLocalizer::localizeDateTime( const QDateTime& dateTime, QLocale::FormatType formatType ) const
{
  Q_UNUSED( formatType )
  return dateTime.toString();
}

QString NullLocalizer::localizeNumber( int number ) const
{
  return QString::number( number );
}

QString NullLocalizer::localizeNumber( qreal number ) const
{
  return QString::number( number );
}

QString NullLocalizer::localizeMonetaryValue( qreal value, const QString& currencyCode ) const
{
  Q_UNUSED( currencyCode )
  return QString::number( value );
}

static void replacePercentN( QString *result, int n )
{
  if ( n >= 0 ) {
    int percentPos = 0;
    int len = 0;
    while ( ( percentPos = result->indexOf( QLatin1Char( '%' ), percentPos + len ) ) != -1 ) {
      len = 1;
      QString fmt;
      if ( result->at( percentPos + len ) == QLatin1Char( 'L' ) ) {
        ++len;
        fmt = QLatin1String( "%L1" );
      } else {
        fmt = QLatin1String( "%1" );
      }
      if ( result->at( percentPos + len ) == QLatin1Char( 'n' ) ) {
        fmt = fmt.arg( n );
        ++len;
        result->replace( percentPos, len, fmt );
        len = fmt.length();
      }
    }
  }
}

static QString substituteArguments( const QString &input, const QVariantList &arguments )
{
  QString string = input;
  Q_FOREACH( const QVariant &arg, arguments ) {
    if ( arg.type() == QVariant::Int )
      string = string.arg( arg.toInt() );
    else if ( arg.type() == QVariant::Double )
      string = string.arg( arg.toDouble() );
    else if ( arg.type() == QVariant::DateTime )
      string = string.arg( arg.toDateTime().toString() );
    else
      string = string.arg( arg.toString() );
  }
  return string;
}

QString NullLocalizer::localizeContextString( const QString& string, const QString& context, const QVariantList &arguments ) const
{
  Q_UNUSED( context )
  return substituteArguments( string, arguments );
}

QString NullLocalizer::localizePluralContextString( const QString& _string, const QString& _pluralForm, const QString& context, const QVariantList &_arguments ) const
{
  Q_UNUSED( context )
  const int count = _arguments.first().toInt();
  QVariantList arguments = _arguments;
  QString string = _string;
  QString pluralForm = _pluralForm;
  if ( _string.contains( QLatin1String( "%n" ) ) ) {
    arguments.removeFirst();
    replacePercentN( &string, count );
    replacePercentN( &pluralForm, count );
  }
  return count > 0 ? substituteArguments( pluralForm, arguments ) : substituteArguments( string, arguments );
}

QString NullLocalizer::localizeString( const QString& string, const QVariantList &arguments ) const
{
  return substituteArguments( string, arguments );
}

QString NullLocalizer::localizePluralString( const QString& _string, const QString& _pluralForm, const QVariantList &_arguments ) const
{
  const int count = _arguments.first().toInt();
  QVariantList arguments = _arguments;
  QString string = _string;
  QString pluralForm = _pluralForm;
  if ( _string.contains( QLatin1String( "%n" ) ) ) {
    arguments.removeFirst();
    replacePercentN( &string, count );
    replacePercentN( &pluralForm, count );
  }
  return count > 0 ? substituteArguments( pluralForm, arguments ) : substituteArguments( string, arguments );

}

QString NullLocalizer::currentLocale() const
{
  return QString();
}

void NullLocalizer::pushLocale( const QString &localeName )
{
  Q_UNUSED( localeName )
}

void NullLocalizer::popLocale()
{

}

void NullLocalizer::loadCatalog( const QString &path, const QString& catalog )
{
  Q_UNUSED( path )
  Q_UNUSED( catalog )
}

void NullLocalizer::unloadCatalog( const QString& catalog )
{
  Q_UNUSED( catalog )
}
