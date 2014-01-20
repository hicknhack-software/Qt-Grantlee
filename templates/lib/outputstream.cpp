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

#include "outputstream.h"

#include "safestring.h"

#include <QRegExp>

using namespace Grantlee;


OutputStream::OutputStream()
  : m_stream( 0 )
  , m_options( SKIP_EMPTY_LINES )
  , m_hasLastLineContent( true )
{

}

OutputStream::OutputStream(QTextStream *stream , Options options)
  : m_stream( stream )
  , m_options( options )
  , m_hasLastLineContent( true )
{

}

OutputStream::~OutputStream()
{

}

QString OutputStream::escape( const QString &input ) const
{
  QString temp = input;
  temp.replace( QLatin1Char( '&' ), QLatin1String( "&amp;" ) );
  temp.replace( QLatin1Char( '<' ), QLatin1String( "&lt;" ) );
  temp.replace( QLatin1Char( '>' ), QLatin1String( "&gt;" ) );
  temp.replace( QLatin1Char( '\'' ), QLatin1String( "&#39;" ) );
  return temp;
}

QString OutputStream::escape( const Grantlee::SafeString& input ) const
{
  return escape( input.get() );
}

QString OutputStream::conditionalEscape( const Grantlee::SafeString& input ) const
{
  if ( !input.isSafe() )
    return escape( input.get() );
  return input;
}

QSharedPointer<OutputStream> OutputStream::clone( QTextStream *stream ) const
{
  return QSharedPointer<OutputStream>( new OutputStream( stream ) );
}


OutputStream& OutputStream::operator<<( const QString& input )
{
  if( m_stream )
    append( input );
  return *this;
}

OutputStream& OutputStream::operator<<( const Grantlee::SafeString& input )
{
  if ( m_stream )
    append( input.needsEscape() ? escape( input.get() ) : (QString)input.get() );
  return *this;
}
/*
OutputStream& OutputStream::operator<<(const Grantlee::OutputStream::Escape& e)
{
  ( *m_stream ) << escape( e.m_content );
  return *this;
}*/

OutputStream& OutputStream::operator<<( QTextStream* stream )
{
  if ( m_stream )
    append( stream->readAll() );
    return *this;
}
/*
Grantlee::OutputStream::MarkSafe::MarkSafe(const QString& input)
  : m_safe( false ), m_content( input )
{

}

Grantlee::OutputStream::MarkSafe::MarkSafe(const Grantlee::SafeString& input)
  : m_safe( input.isSafe() ), m_content( input.get() )
{

}
*/

void OutputStream::append( QString text )
{
  if ( text.isEmpty() )
    return;

  if ( m_options.testFlag(KEEP_ALL_LINES) ) {
    ( *m_stream ) << text;
    return;
  }

  int firstNewlineIndex = text.indexOf("\n");
  if ( -1 == firstNewlineIndex ) {
    appendLastLine( text );
    return;
  }

  appendLastLine( text.left(firstNewlineIndex + 1) );
  text = text.mid(firstNewlineIndex + 1);

  resetLastLine();

  int lastNewlineIndex = text.lastIndexOf("\n");
  if ( -1 == lastNewlineIndex ) {
    appendLastLine( text );
    return;
  }

  ( *m_stream ) << text.left( lastNewlineIndex + 1 );
  appendLastLine( text.mid( lastNewlineIndex + 1 ) );
}

void OutputStream::appendLastLine(const QString &linePart)
{
  static QRegExp non_whitespace("[^\\n\\s]");

  m_lastLine += linePart;

  if( m_hasLastLineContent ) {
    ( *m_stream ) << linePart;
  }
  else {
    m_hasLastLineContent = linePart.contains(non_whitespace);
    if( m_hasLastLineContent ) {
      ( *m_stream ) << m_lastLine;
    }
  }
}

void OutputStream::resetLastLine()
{
  m_lastLine = QString();
  m_hasLastLineContent = false;
}
