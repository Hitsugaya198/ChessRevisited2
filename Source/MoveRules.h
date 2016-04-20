///
/// \file   MoveRules.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains structure definitions and members of the MoveRules class.
///

#ifndef MOVERULES_H
#define MOVERULES_H

#include "Pieces.h"
#include "Colors.h"
#include "CommonTypeDefinitions.h"
#include "Direction.h"
#include "Magnitude.h"

#include <QObject>

///
/// The MoveRules class serves to disperse the movements allowed for each type of Chess Piece.
///
class MoveRules : public QObject
{

  Q_OBJECT

public:

  ///
  /// MoveRules::MoveRules is the default constructor for the MoveRules class.
  ///
  /// \param parent is the QObject that will eventually destroy the pointer to MoveRules. If it is not null.
  ///
  explicit MoveRules(QObject* parent = 0);

  ///
  /// MoveRules::~MoveRules is the default destructor for objects of type MoveRules.
  ///
  virtual ~MoveRules();

  ///
  /// MoveRules::getMovementRules is the function that provides rules regarding direction and magnitude per Piece.
  ///
  /// The color is mainly important for pieces like Pawns that can only move "forward" and diagonally "forward".
  ///
  /// \param identity is the identity of the Piece that requires rules.
  /// \param pieceColor is the color of the Piece that requires rules.
  /// \return the map of rules that govern movements of Chess pieces.
  ///
  static movementType getMovementRules(Pieces::Identities::eIdentities identity, PieceColors::ePieceColors pieceColor);
};

#endif // MOVERULES_H
