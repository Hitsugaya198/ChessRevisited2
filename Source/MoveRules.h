#ifndef MOVERULES_H
#define MOVERULES_H

#include "Pieces.h"

#include <QObject>
#include <QSet>

class MoveRules : public QObject
{
  Q_OBJECT

public:


  struct Direction {
    enum eDirectionRules {
      eMayMoveNorth,
      eMayMoveNorthEast,
      eMayMoveEast,
      eMayMoveSouthEast,
      eMayMoveSouth,
      eMayMoveSouthWest,
      eMayMoveWest,
      eMayMoveNorthWest,
      eInit = eMayMoveNorth,
      eMax = eMayMoveNorthWest
    };
  };

  struct Magnitude {
    enum magnitudes {
      eOne = 1,
      eTwo = 2,

    };
  };

  typedef int magnitudeType;
  typedef QSet < Direction::eDirectionRules > directionType;
  typedef QPair < directionType, directionType > directionsType;
  typedef QPair < magnitudeType, magnitudeType > magnitudesType;
  typedef QPair < directionsType, magnitudesType > movementType;
  typedef Pieces::Identities::eIdentities eIdentities;
  typedef Pieces::PieceColors::ePieceColors ePieceColors;

  explicit MoveRules(QObject* parent = 0);
  virtual ~MoveRules();

  static movementType getMovementRules(eIdentities identity, ePieceColors pieceColor);


signals:

public slots:


};

#endif // MOVERULES_H
