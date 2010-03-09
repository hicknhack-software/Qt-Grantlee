/*
  This file is part of the Grantlee template system.

  Copyright (c) 2009,2010 Stephen Kelly <steveire@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either version
  2 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "cycle.h"

#include <QStringList>
#include "parser.h"
#include "exception.h"

#include "util_p.h"

static const char * _namedCycleNodes = "_namedCycleNodes";

CycleNodeFactory::CycleNodeFactory()
{

}

Node* CycleNodeFactory::getNode( const QString &tagContent, Parser *p ) const
{
  QStringList expr = smartSplit( tagContent );

  if ( expr.size() < 1 ) {
    throw Grantlee::Exception( TagSyntaxError, QString( "%1 expects at least one argument" ).arg( expr.at( 0 ) ) );
  }

  if ( expr.at( 1 ).contains( ',' ) ) {
    QStringList csvlist = expr.at( 1 ).split( ',' );
    expr.removeAt( 1 );
    for ( int i = 0; i < csvlist.size() ; ++i ) {
      expr.insert( i + 1, '"' + csvlist.at( i ) + '"' );
    }
  }

  if ( expr.size() == 2 ) {
    // {% cycle var %}
    QString name = expr.at( 1 );
    QVariant cycleNodes = p->property( _namedCycleNodes );
    if ( !cycleNodes.isValid() || cycleNodes.type() != QVariant::Hash ) {
      throw Grantlee::Exception( TagSyntaxError, QString( "No named cycles in template. '%1' is not defined" ).arg( name ) );
    }
    QVariantHash hash = cycleNodes.toHash();
    if ( !hash.contains( name ) ) {
      throw Grantlee::Exception( TagSyntaxError, QString( "Node not found: %1" ).arg( name ) );
    }
    QVariant nodeVariant = hash.value( name );
    if ( nodeVariant.userType() != QMetaType::QObjectStar ) {
      throw Grantlee::Exception( TagSyntaxError, "Invalid object in node cycle list" );
    }
    QObject *obj = nodeVariant.value<QObject*>();
    Node *node = qobject_cast<Node*>( obj );
    if ( !node ) {
      throw Grantlee::Exception( TagSyntaxError, "Invalid object in node cycle list" );
    }
    return node;
  }

  int exprSize = expr.size();
  if ( exprSize > 4 && expr.at( exprSize - 2 ) == "as" ) {
    // {% cycle "foo" "bar" "bat" as var %}
    QString name = expr.at( exprSize - 1 );
    QStringList list = expr.mid( 1, exprSize - 3 );
    Node *node = new CycleNode( getFilterExpressionList( list, p ), name, p );
    QVariant hashVariant = p->property( _namedCycleNodes );
    QVariantHash hash;
    if ( hashVariant.isValid() && hashVariant.type() == QVariant::Hash ) {
      hash = hashVariant.toHash();
    }
    QObject *nodeObject = node;
    QVariant nodeVariant = QVariant::fromValue( nodeObject );
    hash.insert( name, nodeVariant );
    p->setProperty( _namedCycleNodes, QVariant( hash ) );
    return node;
  } else {
    QStringList list = expr.mid( 1, exprSize - 1 );
    return new CycleNode( getFilterExpressionList( list, p ), QString(), p );
  }
}

CycleNode::CycleNode( QList<FilterExpression> list, const QString &name, QObject *parent )
    : Node( parent ), m_variableIterator( list )
{
  m_name = name;
}

void CycleNode::render( OutputStream *stream, Context *c )
{
  QString value;
  QTextStream textStream( &value );
  QSharedPointer<OutputStream> temp = stream->clone( &textStream );

  m_variableIterator.next().resolve( temp.data(), c );
  if ( !m_name.isEmpty() ) {
    c->insert( m_name, value );
  }
  ( *stream ) << value;
}

