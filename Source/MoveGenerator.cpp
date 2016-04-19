/**
 * @file   MoveGenerator.cpp
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the inner management features of the MoveGenerator class
 *
 * In this cpp file is housed all the functions and attributes needed to manage a
 * a the moves of an AI player during a Chess game.
 */

#include "MoveGenerator.h"
#include "TurnManager.h"
#include "Cell.h"
#include "Piece.h"

///
/// \brief MoveGenerator::_moveStrategy is a static member that determines which function is called on a new move
///
int MoveGenerator::_moveStrategy = 2;

///
/// \brief MoveGenerator::_reverseIterateThroughPieces is a static member.
/// It determines whether a container of pieces is accessed from the front or the back.
///
bool MoveGenerator::_reverseIterateThroughPieces = true;

///
/// \brief MoveGenerator::MoveGenerator is the default constructor for objects of the MoveGenerator class.
/// \param parent is the QObject that will eventually destroy the pointer to MoveGenerator (if not null).
///
MoveGenerator::MoveGenerator(QObject* parent) : QObject(parent)
{

}

///
/// \brief MoveGenerator::~MoveGenerator is the default destructor for objects of type MoveGenerator
///
MoveGenerator::~MoveGenerator()
{

}

///
/// \brief MoveGenerator::handleTurnChange is the function that gets invoked when it is the AI's turn to move
/// \param itIsNowThisPlayersTurn indicates whose turn it is to move.
/// \param containerOfPossibleMoves is a container of moves that proves a move can be made.
/// \param kingIsChecked indicates whether the king of the player whose turn it is, is checked.
///
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

///
/// \brief MoveGenerator::handleMoveCompletionRequired is called after a move has been initiated, in order to complete the move
///
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

///
/// \brief MoveGenerator::moveTheFirstPieceThatCanMove does as its name suggests.  In some cases, priority is given to moves that can capture enemy pieces.
/// \param itIsNowThisPlayersTurn is the player whose turn it is to play.
/// \param containerOfPossibleMoves is a container used for keeping track of what is possible.
/// \param kingIsChecked is an indication of whether moves will be restricted to that which will uncheck the king.
/// \param priorityForAttack is an indication of whether priority is to capture an enemy piece or not.
///
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

///
/// \brief MoveGenerator::moveWithPriorityForAttack wrapper to call moveTheFirstPieceThatCanMove with.
/// \param itIsNowThisPlayersTurn is the player whose turn it is.
/// \param containerOfPossibleMoves is a container used for keeping track of what is possible.
/// \param kingIsChecked is an indication of whether moves will be restricted to that which will uncheck the king.
///
void MoveGenerator::moveWithPriorityForAttack(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                              boardCoordinatesType& containerOfPossibleMoves,
                                              bool kingIsChecked)
{
  moveTheFirstPieceThatCanMove(itIsNowThisPlayersTurn,
                               containerOfPossibleMoves,
                               kingIsChecked,
                               true);
}

///
/// \brief MoveGenerator::moveAnyPieceThatCanMove
/// \param itIsNowThisPlayersTurn is the player whose turn it is.
/// \param containerOfPossibleMoves is a container used for keeping track of what is possible.
/// \param kingIsChecked is an indication of whether moves will be restricted to that which will uncheck the king.
///
void MoveGenerator::moveAnyPieceThatCanMove(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                            boardCoordinatesType& containerOfPossibleMoves,
                                            bool kingIsChecked)
{
  moveTheFirstPieceThatCanMove(itIsNowThisPlayersTurn,
                               containerOfPossibleMoves,
                               kingIsChecked);
}

///
/// \brief MoveGenerator::associatedGameBoard is an accessor method to the game Board associated with this MoveGenerator instance.
/// \return the currently associated Board as a pointer.
///
Board* MoveGenerator::associatedGameBoard() const
{
  return _theGameBoard;
}

///
/// \brief MoveGenerator::associateGameBoard is a mutator method that associates a game Board with this MoveGenerator instance.
/// \param theGameBoard is the Board pointer that will henceforth be associated to this MoveGenerator instance.
///
void MoveGenerator::associateGameBoard(Board* theGameBoard)
{
  _theGameBoard = theGameBoard;
}

///
/// \brief MoveGenerator::aiPlayer is an accessor method to the AI Player associated with this MoveGenerator instance.
/// \return the currently associated AI player for this MoveGenerator instance.
///
QSharedPointer<Player> MoveGenerator::aiPlayer() const
{
  return _aiPlayer;
}

///
/// \brief MoveGenerator::setAiPlayer is a mutator method that associates a Player instance with this MoveGenerator instance.
/// \param aiPlayer is the Player instance that will henceforth be associated with this MoveGenerator instance.
///
void MoveGenerator::setAiPlayer(const QSharedPointer<Player>& aiPlayer)
{
  _aiPlayer = aiPlayer;
}
