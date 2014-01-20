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

#include "customtyperegistry_p.h"

#include "metaenumvariable_p.h"
#include "safestring.h"

#include <QtCore/QDebug>
#include <QtCore/QStack>
#include <QtCore/QStringList>
#include <QtCore/QQueue>

using namespace Grantlee;

CustomTypeRegistry::CustomTypeRegistry()
{
  // Grantlee Types
  registerBuiltInMetatype<SafeString>();
  registerBuiltInMetatype<MetaEnumVariable>();
}

void CustomTypeRegistry::registerLookupOperator( int id, MetaType::LookupFunction f )
{
  CustomTypeInfo &info = types[id];
  info.lookupFunction = f;
}

MetaType::LookupFunction CustomTypeRegistry::findConverterLookup( int objectType ) const
{
  QHashIterator<int, CustomTypeInfo> i(types);
  while ( i.hasNext() ) {
    i.next();
    if ( 0 != i.value().lookupFunction
        && QMetaType::hasRegisteredConverterFunction(objectType, i.key()) ) {
      return i.value().lookupFunction;
    }
  }
  return 0;
}

QVariant CustomTypeRegistry::lookup( const QVariant &object, const QString &property ) const
{
  int objectType = object.userType();
  MetaType::LookupFunction lf = types.value(objectType).lookupFunction;
  if ( 0 == lf ) {
    lf = findConverterLookup( objectType );
  }
  if ( 0 == lf ) {
    qWarning() << "No lookup function for metatype" << QMetaType::typeName( objectType );
    return QVariant();
  }

  return lf( object, property );
}

bool CustomTypeRegistry::lookupAlreadyRegistered( int id ) const
{
  return types.contains( id ) && types.value( id ).lookupFunction != 0;
}
