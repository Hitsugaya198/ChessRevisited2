///
/// \file   MoveGenerator.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the inner management features of the MoveGenerator class.
///
/// In this cpp file is housed all the functions and attributes needed to manage a
/// a the moves of an AI Player during a Chess game.
///

#include "MoveGenerator.h"
#include "TurnManager.h"
#include "Cell.h"
#include "Piece.h"

int MoveGenerator::_moveStrategy = 2;
bool MoveGenerator::_reverseIterateThroughPieces = true;

MoveGenerator::MoveGenerator(QObject* parent) : QObject(parent)
{

}

MoveGenerator::~MoveGenerator()
{

}

void MoveGenerator::handleTurnChange(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                     boardCoordinatesType& containerOfPossibleMoves,
                                     bool kingIsChecked)
{
  if (itIsNowThisPlayersTurn->identity() == _aiPlayer->identity()) {
    if (_moveStrategy == 2) {
      moveWithPriorityForAttack(itIsNowThisPlayersTurn, containerOfPossibleMoves, kingIsChecked);
    }
    else if (_moveStrategy == 4) {
      moveAnyPieceThatCanMove(itIsNowThisPlayersTurn, containerOfPossibleMoves, kingIsChecked);
    }

    if (_moveStrategy != 4) {
      _moveStrategy += 2;
    }
    else {
      _moveStrategy = 2;
    }
  }
}

void MoveGenerator::handleMoveCompletionRequired()
{
  if (!_containerForMoving.isEmpty()) {
    if (_locationEnd.first <= 0 || _locationEnd.first > 8 ||
        _locationEnd.second <= 0 || _locationEnd.second > 8) {
      _locationEnd = *(_containerForMoving.begin());
    }
    _theGameBoard->toggleCell(_theGameBoard->getCell(_locationEnd));
    _locationEnd = boardCoordinateType();
  }
}

void MoveGenerator::moveTheFirstPieceThatCanMove(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                                 boardCoordinatesType& containerOfPossibleMoves,
                                                 bool kingIsChecked,
                                                 bool priorityForAttack)
{
  bool kingChecked = kingIsChecked;

  if (priorityForAttack) { // first try to attack
    if (MoveMapper::getInstance().doesPlayerHaveAvailableMoves(itIsNowThisPlayersTurn,
                                                               containerOfPossibleMoves,
                                                               &kingChecked,
                                                               _locationStart,
                                                               _reverseIterateThroughPieces,
                                                               true)) {
      _locationEnd = *(containerOfPossibleMoves.begin());
    } // if no attack moves are available, move any piece
    else if (MoveMapper::getInstance().doesPlayerHaveAvailableMoves(itIsNowThisPlayersTurn,
                                                                    containerOfPossibleMoves,
                                                                    &kingChecked,
                                                                    _locationStart,
                                                                    _reverseIterateThroughPieces,
                                                                    false)) {
      _locationEnd = *(containerOfPossibleMoves.begin());
    }
    else {
      // END GAME!
    }
  }
  else { // just move any piece without trying to attack
    if (MoveMapper::getInstance().doesPlayerHaveAvailableMoves(itIsNowThisPlayersTurn,
                                                               containerOfPossibleMoves,
                                                               &kingChecked,
                                                               _locationStart,
                                                               _reverseIterateThroughPieces,
                                                               false)) {
      _locationEnd = *(containerOfPossibleMoves.begin());
    }
    else {
      // END GAME!
    }
  }


  if (containerOfPossibleMoves.isEmpty()) {
    _theGameBoard->clearHighLights();
    _theGameBoard->uncheckAllCheckedCells();
  }
  else {
    _theGameBoard->highLightCoordinates(containerOfPossibleMoves);
    // select the first move and go for it.
    _containerForMoving = containerOfPossibleMoves;
    _theGameBoard->toggleCell(_theGameBoard->getCell(_locationStart));
  }
  _reverseIterateThroughPieces = !_reverseIterateThroughPieces;
}

void MoveGenerator::moveWithPriorityForAttack(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                              boardCoordinatesType& containerOfPossibleMoves,
                                              bool kingIsChecked)
{
  moveTheFirstPieceThatCanMove(itIsNowThisPlayersTurn,
                               containerOfPossibleMoves,
                               kingIsChecked,
                               true);
}

void MoveGenerator::moveAnyPieceThatCanMove(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                            boardCoordinatesType& containerOfPossibleMoves,
                                            bool kingIsChecked)
{
  moveTheFirstPieceThatCanMove(itIsNowThisPlayersTurn,
                               containerOfPossibleMoves,
                               kingIsChecked);
}

Board* MoveGenerator::associatedGameBoard() const
{
  return _theGameBoard;
}

void MoveGenerator::associateGameBoard(Board* theGameBoard)
{
  _theGameBoard = theGameBoard;
}

QSharedPointer<Player> MoveGenerator::aiPlayer() const
{
  return _aiPlayer;
}

void MoveGenerator::setAiPlayer(const QSharedPointer<Player>& aiPlayer)
{
  _aiPlayer = aiPlayer;
}
