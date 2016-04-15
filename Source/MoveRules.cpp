#include "MoveRules.h"

///
/// \brief MoveRules::MoveRules
/// \param parent
///
MoveRules::MoveRules(QObject* parent) : QObject(parent)
{

}

///
/// \brief MoveRules::~MoveRules
///
MoveRules::~MoveRules()
{

}

///
/// \brief MoveRules::getMovementRules
/// \param identity
/// \param pieceColor
/// \return
///
MoveRules::movementType MoveRules::getMovementRules(eIdentities identity, ePieceColors pieceColor)
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
    if (pieceColor == Pieces::PieceColors::eBlack) { // Plays north to south
      direction1.insert(Direction::eMayMoveSouth);
      direction1.insert(Direction::eMayMoveSouthEast);
      direction1.insert(Direction::eMayMoveSouthWest);
    }
    else { // Plays south to north
      direction1.insert(Direction::eMayMoveNorth);
      direction1.insert(Direction::eMayMoveNorthEast);
      direction1.insert(Direction::eMayMoveNorthWest);
    }

    magnitude1 = 1;
    magnitude2 = 2; // Special Case: Only from starting position
    break;
  }
  case Pieces::Identities::eBishop : {
    // Color not relevant.
    direction1.insert(Direction::eMayMoveNorthEast);
    direction1.insert(Direction::eMayMoveNorthWest);
    direction1.insert(Direction::eMayMoveSouthEast);
    direction1.insert(Direction::eMayMoveSouthWest);
    magnitude1 = 1;
    magnitude2 = 8;
    break;
  }
  case Pieces::Identities::eCastle : {
    // Color not relevant.
    direction1.insert(Direction::eMayMoveNorth);
    direction1.insert(Direction::eMayMoveWest);
    direction1.insert(Direction::eMayMoveEast);
    direction1.insert(Direction::eMayMoveSouth);
    magnitude1 = 1;
    magnitude2 = 8;
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
    magnitude1 = 1;
    magnitude2 = 1;
    break;
  }
  case Pieces::Identities::eKnight : {
    direction1.insert(Direction::eMayMoveNorthEast);
    direction1.insert(Direction::eMayMoveNorthWest);
    direction1.insert(Direction::eMayMoveSouthEast);
    direction1.insert(Direction::eMayMoveSouthWest);
    magnitude1 = 3;
    magnitude2 = 3;
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
    magnitude1 = 1;
    magnitude2 = 8;
    break;
  }
  }
  directions = directionsType(direction1, direction2);
  magnitudes = magnitudesType(magnitude1, magnitude2);
  movements = movementType(directions, magnitudes);
  return movements;
}
