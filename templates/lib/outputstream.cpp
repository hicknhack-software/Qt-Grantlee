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

using namespace Grantlee;


OutputStream::OutputStream()
  : m_stream( 0 )
{

}

OutputStream::OutputStream( QTextStream *stream )
  : m_stream( stream )
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
	append( input.needsEscape() ? escape( input.get() ) : input.get() );
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

  static QRegExp emptyLineStart("^(\\s*\\n)");
  static QRegExp wsString("^[\\n\\s]*$");
  static QRegExp lastLine("\\n([^\\n]*)$");

  if ( text.isEmpty() )
    return;

  //text is only whitespace
  if ( wsString.indexIn(m_last_line)  != -1 )
  {
    m_last_line = "";
    if( !text.contains("{") )
    {
      text.replace(emptyLineStart, "");
    }
    else  //don't remove line breaks before '{'
    {
      text.replace(QRegExp("^(\\s[^\\n])*\\{(\\s*\\n)"),"{\n");
    }
  }
  else 
  {
    text = m_last_line + text;
  }

  //text is multiline
  //delete last line if empty
  if ( text.contains("\n") ) {
    lastLine.indexIn( text );
    if ( lastLine.captureCount() > 1 ) {
      m_last_line = lastLine.cap(1);
      text.replace( lastLine, "" );
    }
  }

  ( *m_stream ) << text;
}
