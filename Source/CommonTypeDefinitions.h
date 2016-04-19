/**
 * @file   CommonTypeDefinitions.h
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains common typedef data types used throughout the Chess game.
 */

#ifndef COMMONTYPEDEFINITIONS
#define COMMONTYPEDEFINITIONS

#include "Pieces.h"
#include "Colors.h"
#include "Direction.h"
#include "Magnitude.h"

#include <QPair>
#include <QMap>
#include <QSet>

/**
 * Anonymous Namespace
 */
namespace
{
  /**
   * @brief rowType just an integer used as the row indicator of a coordinate
   */
  typedef int rowType;

  /**
   * @brief columnType just an integer used as the column indicator of a coordinate
   */
  typedef int columnType;

  /**
   * @brief boardCoordinateType combined rowType and columnType into a complex type (QPair)
   */
  typedef QPair < rowType, columnType > boardCoordinateType;

  /**
   * @brief boardCoordinatesType a collection of boardCoordinateType values in a QSet
   */
  typedef QSet < boardCoordinateType > boardCoordinatesType;

  /**
   * @brief definedPieceType combines the attributes of a piece into a single type.
   */
  typedef QPair < Pieces::Identities::eIdentities, PieceColors::ePieceColors > definedPieceType;

  /**
   * @brief boardStateMapType the state of a board is mapped as all the playable pieces and their locations
   */
  typedef QMap < boardCoordinateType, definedPieceType > boardStateMapType;

  /**
   * @brief boardStateMapIteratorType is an iterator type for QMap. Useful for reverse-iteration.
   */
  typedef QMapIterator < boardCoordinateType, definedPieceType > boardStateMapIteratorType;

  /**
   * @brief piecesListType a list of pieces, used to document captured pieces
   */
  typedef QList < definedPieceType > piecesListType;

  /**
   * @brief magnitudeType is a custom, limited numerical definition from 1 to 8
   */
  typedef Magnitude::eMagnitudes magnitudeType;

  /**
   * @brief directionType is a custom directions defined type, based on the wind direction
   */
  typedef QSet < Direction::eDirectionRules > directionType;

  /**
   * @brief directionsType is a container type that houses elements of type directionType
   */
  typedef QPair < directionType, directionType > directionsType;

  /**
   * @brief magnitudesType is a containertype that houses elements of type magnitudeType
   */
  typedef QPair < magnitudeType, magnitudeType > magnitudesType;

  /**
   * @brief movementType is a defined type that equates to a physics vector, has direction and magnitude.
   */
  typedef QPair < directionsType, magnitudesType > movementType;
}

#endif // COMMONTYPEDEFINITIONS

