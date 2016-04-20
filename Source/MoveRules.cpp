///
/// \file   MoveRules.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the functionality to map movement rules for Chess Pieces.
///
/// In this cpp file is housed the single function needed to construct a
/// a map of rules that contain  directions and magnitudes for a given type
/// of Chess piece.
///
///

#include "MoveRules.h"

MoveRules::MoveRules(QObject* parent) : QObject(parent)
{

}

MoveRules::~MoveRules()
{

}

movementType MoveRules::getMovementRules(Pieces::Identities::eIdentities identity,
                                         PieceColors::ePieceColors pieceColor)
{
  magnitudesType magnitudes;
  magnitudeType magnitude1;
  magnitudeType magnitude2;
  directionsType directions;
  directionType direction1;
  directionType direction2;
  movementType movements;

  switch (identity) {
  case Pieces::Identities::ePawn   : {
    if (pieceColor == PieceColors::eBlack) { // Plays north to south
      direction1.insert(Direction::eMayMoveSouth);
      direction1.insert(Direction::eMayMoveSouthEast);
      direction1.insert(Direction::eMayMoveSouthWest);
    }
    else { // Plays south to north
      direction1.insert(Direction::eMayMoveNorth);
      direction1.insert(Direction::eMayMoveNorthEast);
      direction1.insert(Direction::eMayMoveNorthWest);
    }

    magnitude1 = Magnitude::eOne;
    magnitude2 = Magnitude::eTwo; // Special Case: Only from starting position
    break;
  }
  case Pieces::Identities::eBishop : {
    // Color not relevant.
    direction1.insert(Direction::eMayMoveNorthEast);
    direction1.insert(Direction::eMayMoveNorthWest);
    direction1.insert(Direction::eMayMoveSouthEast);
    direction1.insert(Direction::eMayMoveSouthWest);
    magnitude1 = Magnitude::eOne;
    magnitude2 = Magnitude::eEight;
    break;
  }
  case Pieces::Identities::eCastle : {
    // Color not relevant.
    direction1.insert(Direction::eMayMoveNorth);
    direction1.insert(Direction::eMayMoveWest);
    direction1.insert(Direction::eMayMoveEast);
    direction1.insert(Direction::eMayMoveSouth);
    magnitude1 = Magnitude::eOne;
    magnitude2 = Magnitude::eEight;
    break;
  }
  case Pieces::Identities::eKing   : {
    // Color not relevant.
    direction1.insert(Direction::eMayMoveNorth);
    direction1.insert(Direction::eMayMoveWest);
    direction1.insert(Direction::eMayMoveEast);
    direction1.insert(Direction::eMayMoveSouth);
    direction1.insert(Direction::eMayMoveNorthEast);
    direction1.insert(Direction::eMayMoveNorthWest);
    direction1.insert(Direction::eMayMoveSouthEast);
    direction1.insert(Direction::eMayMoveSouthWest);
    magnitude1 = Magnitude::eOne;
    magnitude2 = Magnitude::eOne;
    break;
  }
  case Pieces::Identities::eKnight : {
    direction1.insert(Direction::eMayMoveNorthEast);
    direction1.insert(Direction::eMayMoveNorthWest);
    direction1.insert(Direction::eMayMoveSouthEast);
    direction1.insert(Direction::eMayMoveSouthWest);
    magnitude1 = Magnitude::eThree;
    magnitude2 = Magnitude::eThree;
    break;
    // This one is going to be tricky, as the magnitudes are interchangeable
  }
  case Pieces::Identities::eQueen  : {
    // Color not relevant.
    direction1.insert(Direction::eMayMoveNorth);
    direction1.insert(Direction::eMayMoveWest);
    direction1.insert(Direction::eMayMoveEast);
    direction1.insert(Direction::eMayMoveSouth);
    direction1.insert(Direction::eMayMoveNorthEast);
    direction1.insert(Direction::eMayMoveNorthWest);
    direction1.insert(Direction::eMayMoveSouthEast);
    direction1.insert(Direction::eMayMoveSouthWest);
    magnitude1 = Magnitude::eOne;
    magnitude2 = Magnitude::eEight;
    break;
  }
  }
  directions = directionsType(direction1, direction2);
  magnitudes = magnitudesType(magnitude1, magnitude2);
  movements = movementType(directions, magnitudes);
  return movements;
}
