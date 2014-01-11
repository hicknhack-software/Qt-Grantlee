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

#ifndef GRANTLEE_TYPEACCESSOR_H
#define GRANTLEE_TYPEACCESSOR_H

#include "containeraccessor.h"
#include "grantlee_core_export.h"

#include <QtCore/QLinkedList>
#include <QtCore/QSet>
#include <QtCore/QSharedPointer>
#include <QtCore/QVariant>

#include <deque>
#include <list>
#include <vector>

/// @file

namespace Grantlee
{

#ifndef Q_QDOC
template <typename T>
struct TypeAccessor
{
  static QVariant lookUp( const T object, const QString &property );
};

template <typename T>
struct TypeAccessor<T*>
{
  static QVariant lookUp( const T * const object, const QString &property );
};

template <typename T>
struct TypeAccessor<T&>
{
  static QVariant lookUp( const T &object, const QString &property );
};

#endif

#ifndef Q_QDOC
template <>
QVariant GRANTLEE_CORE_EXPORT TypeAccessor<QObject*>::lookUp( const QObject * const object, const QString &property );

/**
 * @internal Looks up the property @p property from QObject @p object.
 */
QVariant GRANTLEE_CORE_EXPORT doQobjectLookUp( const QObject * const object, const QString& property );

typedef QPair<QVariant, QVariant> QVariantPair;

QVariant GRANTLEE_CORE_EXPORT doPairLookUp( const QVariantPair& pair, const QString& property );

QVariant GRANTLEE_CORE_EXPORT doAssociativeLookUp(const QAssociativeIterable& associative, const QString &property );

QVariant GRANTLEE_CORE_EXPORT doSequentialLookUp( const QSequentialIterable& sequence, const QString &property );
#endif
}

/**
  Allows %Grantlee to access SmartPointer&lt;QObjectSubclass&gt; in templates.

  @see @ref smart_pointers
 */
#define GRANTLEE_SMART_PTR_ACCESSOR(SmartPointer)                                  \
namespace Grantlee {                                                               \
template<typename T>                                                               \
struct TypeAccessor<SmartPointer<T>&>                                              \
{                                                                                  \
  static QVariant lookUp( const SmartPointer<T> &object, const QString &property ) \
  {                                                                                \
    return doQobjectLookUp( object.operator->(), property );                       \
  }                                                                                \
};                                                                                 \
}                                                                                  \

GRANTLEE_SMART_PTR_ACCESSOR(QSharedPointer)

#endif
