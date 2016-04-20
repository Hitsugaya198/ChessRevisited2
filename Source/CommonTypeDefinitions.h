///
/// \file   CommonTypeDefinitions.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains common typedef data types used throughout the Chess game.
///

#ifndef COMMONTYPEDEFINITIONS
#define COMMONTYPEDEFINITIONS

#include "Pieces.h"
#include "Colors.h"
#include "Direction.h"
#include "Magnitude.h"
#include "StyleComponents.h"

#include <QPair>
#include <QMap>
#include <QSet>

///
/// Anonymous Namespace
///
namespace
{
  ///
  /// rowType just an integer used as the row indicator of a coordinate.
  ///
  typedef int rowType;

  ///
  /// columnType just an integer used as the column indicator of a coordinate.
  ///
  typedef int columnType;

  ///
  /// boardCoordinateType combined rowType and columnType into a complex type (QPair).
  ///
  typedef QPair < rowType, columnType > boardCoordinateType;

  ///
  /// boardCoordinatesType a collection of boardCoordinateType values in a QSet.
  ///
  typedef QSet < boardCoordinateType > boardCoordinatesType;

  ///
  /// definedPieceType combines the attributes of a piece into a single type.
  ///
  typedef QPair < Pieces::Identities::eIdentities, PieceColors::ePieceColors > definedPieceType;

  ///
  /// boardStateMapType the state of a board is mapped as all the playable pieces and their locations.
  ///
  typedef QMap < boardCoordinateType, definedPieceType > boardStateMapType;

  ///
  /// boardStateMapIteratorType is an iterator type for QMap. Useful for reverse-iteration.
  ///
  typedef QMapIterator < boardCoordinateType, definedPieceType > boardStateMapIteratorType;

  ///
  /// piecesListType a list of pieces, used to document captured pieces.
  ///
  typedef QList < definedPieceType > piecesListType;

  ///
  /// magnitudeType is a custom, limited numerical definition from 1 to 8.
  ///
  typedef Magnitude::eMagnitudes magnitudeType;

  ///
  /// directionType is a custom directions defined type, based on wind directions.
  ///
  typedef QSet < Direction::eDirectionRules > directionType;

  ///
  /// directionsType is a container type that houses elements of type directionType.
  ///
  typedef QPair < directionType, directionType > directionsType;

  ///
  /// magnitudesType is a containertype that houses elements of type magnitudeType.
  ///
  typedef QPair < magnitudeType, magnitudeType > magnitudesType;

  ///
  /// movementType is a defined type that equates to a physics vector, has direction and magnitude.
  ///
  typedef QPair < directionsType, magnitudesType > movementType;

  ///
  /// splitStyleMapType
  ///
  typedef QMap<eStyleComponents, QString> splitStyleMapType;
}

#endif // COMMONTYPEDEFINITIONS

