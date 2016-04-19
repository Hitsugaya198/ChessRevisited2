/**
 * @file   MoveRules.h
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains structure definitions and members of the MoveRules class.
 */

#ifndef MOVERULES_H
#define MOVERULES_H

#include "Pieces.h"
#include "Colors.h"
#include "CommonTypeDefinitions.h"
#include "Direction.h"
#include "Magnitude.h"

#include <QObject>

/**
 * @brief The MoveRules class
 */
class MoveRules : public QObject
{

  Q_OBJECT

public:
  explicit MoveRules(QObject* parent = 0);
  virtual ~MoveRules();

  static movementType getMovementRules(Pieces::Identities::eIdentities identity, PieceColors::ePieceColors pieceColor);
};

#endif // MOVERULES_H
