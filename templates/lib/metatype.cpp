/*
  This file is part of the Grantlee template system.

  Copyright (c) 2010 Michael Jansen <kde@michael-jansen.biz>
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

#include "metatype.h"

#include "customtyperegistry_p.h"
#include "metaenumvariable_p.h"

#include <QtCore/QDebug>

using namespace Grantlee;

Q_GLOBAL_STATIC( CustomTypeRegistry, customTypes )


void Grantlee::MetaType::internalLock()
{
  return customTypes()->mutex.lock();
}

void Grantlee::MetaType::internalUnlock()
{
  return customTypes()->mutex.unlock();
}

void Grantlee::MetaType::registerLookUpOperator( int id, LookupFunction f )
{
  Q_ASSERT( id > 0 );
  Q_ASSERT( f );

  customTypes()->registerLookupOperator( id, f );
}

inline QVariant doQobjectLookUp( const QObject * const object, const QString &property )
{
  if ( ! object ) {
    return QVariant();
  }
  if ( property == QLatin1String( "children" ) ) {
    return QVariant::fromValue( object->children() );
  }
  if ( property == QLatin1String( "objectName" ) ) {
    return object->objectName();
  }
  // Can't be const because of invokeMethod.
  const QMetaObject *metaObj = object->metaObject();

  const QByteArray propertyUtf8 = property.toUtf8();

  QMetaProperty mp;
  for ( int i = 0; i < metaObj->propertyCount(); ++i ) {
    // TODO only read-only properties should be allowed here.
    // This might also handle the variant messing I hit before.
    mp = metaObj->property( i );

    if ( mp.isReadable() && mp.name() == propertyUtf8 ) {
      if ( mp.isEnumType() ) {
        return QVariant::fromValue( MetaEnumVariable( mp.enumerator(), mp.read( object ).toInt() ) );
      }

      return mp.read( object );
    }
  }

  QMetaEnum me;
  for ( int i = 0; i < metaObj->enumeratorCount(); ++i ) {
    me = metaObj->enumerator( i );

    if ( me.name() == propertyUtf8 ) {
      return QVariant::fromValue( MetaEnumVariable(me) );
    }

    bool ok;
    const int value = me.keyToValue( propertyUtf8.constData(), &ok );
    if ( ok ) {
      return QVariant::fromValue( MetaEnumVariable(me, value) );
    }
  }

  return object->property( propertyUtf8.constData() );
}

inline QVariant doSequentialLookUp( const QSequentialIterable &sequence, const QString &property )
{
  if (property == QLatin1String( "size" )
      || property == QLatin1String( "count" ) ) {
    return sequence.size();
  }

  bool ok = false;
  const int listIndex = property.toInt( &ok );

  if ( !ok || listIndex >= sequence.size() ) {
    qWarning() << "Wrong Sequential index:" << property;
    return QVariant();
  }

  return sequence.at(listIndex);
}

inline QVariant doAssociativeLookUp( const QAssociativeIterable &associative, const QString &property )
{
  QVariant mappedValue = associative.value(property);
  if(mappedValue.isValid())
    return mappedValue;

  if (property == QLatin1String("size") || property == QLatin1String( "count" ) ) {
    return associative.size();
  }

  if ( property == QLatin1String( "items" ) ) {
    QAssociativeIterable::const_iterator it = associative.begin();
    const QAssociativeIterable::const_iterator end = associative.end();
    QVariantList list;
    list.reserve(associative.size());
    for ( ; it != end; ++it ) {
      QVariantList nested;
      nested.push_back( it.key() );
      nested.push_back( it.value() );
      list.push_back( nested );
    }
    return list;
  }

  if ( property == QLatin1String( "keys" ) ) {
    QAssociativeIterable::const_iterator it = associative.begin();
    const QAssociativeIterable::const_iterator end = associative.end();
    QVariantList list;
    list.reserve(associative.size());
    for ( ; it != end; ++it ) {
      list.push_back( it.key() );
    }
    return list;
  }

  if ( property == QLatin1String( "values" ) ) {
    QAssociativeIterable::const_iterator it = associative.begin();
    const QAssociativeIterable::const_iterator end = associative.end();
    QVariantList list;
    list.reserve(associative.size());
    for ( ; it != end; ++it ) {
      list.push_back( it.value() );
    }
    return list;
  }

  qWarning() << "Wrong Associative key:" << property;
  return QVariant();
}

QVariant Grantlee::MetaType::lookup( const QVariant &object, const QString &property )
{
  if ( !object.isValid() )
    return QVariant();

  if ( object.canConvert(QMetaType::QObjectStar) )
    return doQobjectLookUp( object.value<QObject*>(), property );
  if ( object.canConvert(QMetaType::QVariantList) )
    return doSequentialLookUp( object.value<QSequentialIterable>(), property );
  if ( object.canConvert(QMetaType::QVariantHash) )
    return doAssociativeLookUp( object.value<QAssociativeIterable>(), property );

  return customTypes()->lookup( object, property );
}

bool Grantlee::MetaType::lookupAlreadyRegistered( int id )
{
  return customTypes()->lookupAlreadyRegistered( id );
}
