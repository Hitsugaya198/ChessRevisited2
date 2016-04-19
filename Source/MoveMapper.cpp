/**
 * @file   MoveMapper.cpp
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the inner management features of the MoveMapper class
 *
 * In this cpp file is housed all the functions and attributes needed to determine
 * whether a player has any availble moves at his disposal.
 */

#include "MoveMapper.h"

#include "Cell.h"
#include "Piece.h"

///
/// \brief MoveMapper::MoveMapper is the default constructor for the MoveMapper type
/// \param parent is the QObject that will eventually destroy the pointer to MoveMapper (if not null).
///
MoveMapper::MoveMapper(QObject* parent) : QObject(parent)
{

}

///
/// \brief MoveMapper::~MoveMapper is the default destructor for MoveMapper objects
///
MoveMapper::~MoveMapper()
{

}

///
/// \brief MoveMapper::doesPlayerHaveAvailableMoves calculates every possible move of every piece for a given player.
/// \param whichPlayer is the player that the calculation will be done for.
/// \param containerOfAvailableMoves [in,out] is the container of moves available, post analysis.
/// \param kingChecked [out] is a boolean that indicates whether the player referenced by whichPlayer's king is checked.
/// \param locationStart [out] is a boardCoordinateType that tells you the location of the piece whose valid moves are in containerOfAvailableMoves.
/// \param reverseIterate is a boolean that indicates whether containers will be accessed from the back or the front.
/// \param priorityForAttack is a boolean that indicates whether or not priority should be given to attack enemy pieces.
/// \return true if moves are available, false if no moves are available to the player.
///
bool MoveMapper::doesPlayerHaveAvailableMoves(QSharedPointer<Player>& whichPlayer,
                                              boardCoordinatesType& containerOfAvailableMoves,
                                              bool* kingChecked,
                                              boardCoordinateType& locationStart,
                                              bool reverseIterate,
                                              bool priorityForAttack)
{
  boardCoordinateType locationOfAttacker;
  boardCoordinateType locationOfVictim;
  definedPieceType    pieceWhoWillBeAttacking;
  definedPieceType    pieceWhoWillBeAttacked;

  // Find a piece that can move, start by retrieving the current board map
  boardStateMapType currentStateOfTheGameBoard = Board::workingBoardStateMap();
  boardCoordinatesType availableMovesContainer = boardCoordinatesType();

  bool boardIsValid = _theGameBoard->evaluateBoardState(currentStateOfTheGameBoard);
  bool hasPreviousOrNext = true;

  // Loop through all the pieces
  boardStateMapIteratorType boardIterator(currentStateOfTheGameBoard);
  if (!reverseIterate) {
    boardIterator.toFront();
    hasPreviousOrNext = boardIterator.hasNext();
  }
  else {
    boardIterator.toBack();
    hasPreviousOrNext = boardIterator.hasPrevious();
  }

  while (hasPreviousOrNext) {


    if (!reverseIterate) {
      boardIterator.next();
      hasPreviousOrNext = boardIterator.hasNext();
    }
    else {
      boardIterator.previous();
      hasPreviousOrNext = boardIterator.hasPrevious();
    }

    // look at the piece...
    boardCoordinateType currentPieceLocation = boardIterator.key();
    definedPieceType    currentPiece = boardIterator.value();

    // If it is not the player's color, don't allow a move
    if (currentPiece.second != whichPlayer->associatedColor()) {
      continue;
    }

    // This would imply the King of the current player is checked, so valid moves should save him
    if (!boardIsValid) {
      // see if any potential moves can bring the board into a legal state (uncheck the king)
      // first see if the selected cell can attack the offender, unless it is the king, and he is further
      // than one cell away.

      movementType rules = MoveRules::getMovementRules(currentPiece.first, currentPiece.second);
      boardCoordinatesType container = boardCoordinatesType();

      // Map it's possible moves.  _theGameBoard->mapMoves() only returns "legal" moves
      _theGameBoard->mapMoves(rules, currentPiece, container, currentPieceLocation, currentStateOfTheGameBoard);

      if (!container.isEmpty()) {
        boardCoordinatesType set;
        bool canItBeAttacked = _theGameBoard->isTheTargetWithinRange(PieceColors::flipColor(whichPlayer->associatedColor()),
                                                                     _theGameBoard->pieceWhoWillBeAttacking().first,
                                                                     set,
                                                                     currentStateOfTheGameBoard,
                                                                     locationOfAttacker,
                                                                     locationOfVictim,
                                                                     pieceWhoWillBeAttacking,
                                                                     pieceWhoWillBeAttacked);
        if (canItBeAttacked) {
          // Attack it from anywhere

          // But first make sure it is not going to result in a broken board state
          Cell* from = _theGameBoard->getCell(locationOfAttacker);
          Cell* to = _theGameBoard->getCell(locationOfVictim);

          boardStateMapType scenario = boardStateMapType(currentStateOfTheGameBoard);
          piecesListType scenarioPieces = piecesListType(_theGameBoard->workingCapturedPieces());

          _theGameBoard->movePieceStart(_theGameBoard, from, to, scenario, scenarioPieces);

          bool boardStillValid = _theGameBoard->evaluateBoardState(scenario);

          _theGameBoard->movePieceRevertMove(scenario, scenarioPieces);

          if (boardStillValid) { // attacking won't keep the king in a compromised state
            locationStart = locationOfAttacker;
            availableMovesContainer = set;
            break;
          }
          else { // attacking will keep the king in a compromised state
            continue;
          }
        }
        else {
          // Try to block its path
          boardCoordinatesType set = _theGameBoard->getPath(_theGameBoard->locationOfVictim(), _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);
          boardCoordinatesType possibleMoves;
          if (currentPiece.first == Pieces::Identities::eKing) {
            // If the current piece is the king, and we cannot attack, we must run away
            possibleMoves = container.subtract(set);
          }
          else {
            possibleMoves = set.intersect(container);
          }

          if (!possibleMoves.isEmpty()) {

            // Iterate through the possible moves to find one that leaves the board in a legal state
            boardCoordinatesType::iterator possibleMovesIterator = possibleMoves.begin();
            Cell* from = _theGameBoard->getCell(currentPieceLocation);
            bool breakOutOfOuterWhile = false;
            while (possibleMovesIterator != possibleMoves.end()) {
              Cell* to = _theGameBoard->getCell(*possibleMovesIterator);
              ++possibleMovesIterator;

              boardStateMapType scenario = boardStateMapType(currentStateOfTheGameBoard);
              piecesListType scenarioPieces = piecesListType(_theGameBoard->workingCapturedPieces());

              _theGameBoard->movePieceStart(_theGameBoard, from, to, scenario, scenarioPieces);

              bool boardStillValid = _theGameBoard->evaluateBoardState(scenario);

              _theGameBoard->movePieceRevertMove(scenario, scenarioPieces);

              if (boardStillValid) {
                locationStart = currentPieceLocation;
                availableMovesContainer = possibleMoves;
                breakOutOfOuterWhile = true;
                break; // while(possibleMovesIterator != possibleMoves.end())
              }
              else {
                continue; // while(possibleMovesIterator != possibleMoves.end())
              }
            }
            if (breakOutOfOuterWhile) { // while (hasPreviousOrNext)
              break;
            }
          }
          else {
            // Selected piece has no available moves.
          }
        }
      }
    }
    else {
      // Board is in a valid state (your king is not in danger)
      movementType rules = MoveRules::getMovementRules(currentPiece.first, currentPiece.second);
      boardCoordinatesType container = boardCoordinatesType();

      // Map it's possible moves.  _theGameBoard->mapMoves() only returns "legal" moves
      _theGameBoard->mapMoves(rules, currentPiece, container, currentPieceLocation, currentStateOfTheGameBoard);


      if (!container.isEmpty()) {
        // One last check to see if any of the proposed moves will in fact result in an invalid board state.
        boardCoordinatesType::iterator containerIterator = container.begin();
        while (containerIterator != container.end()) {
          boardStateMapType tempState = currentStateOfTheGameBoard;
          piecesListType tempPieces =  _theGameBoard->workingCapturedPieces();
          boardCoordinateType toWhere = *containerIterator;
          Cell* to = _theGameBoard->getCell(toWhere);

          // if there is a priority to attack first, then the end result
          // should only contain moves with targets
          if (priorityForAttack) {
            if (to->assignedPiece()->color() != PieceColors::flipColor(whichPlayer->associatedColor())) {
              // not an enemy piece, so not an attack...
              container.remove(toWhere);
              containerIterator = container.begin();
              continue;
            }
          }

          Cell* from = _theGameBoard->getCell(currentPieceLocation);
          Board::movePieceStart(_theGameBoard, from, to, tempState, tempPieces);

          bool boardStillValid = _theGameBoard->evaluateBoardState(tempState);
          if (!boardStillValid) { // this will check the current player's king
            container.remove(toWhere);
            containerIterator = container.begin();
          }
          else {
            ++containerIterator;
          }
          Board::movePieceRevertMove(tempState, tempPieces);
        }

        if (!container.isEmpty()) {
          locationStart = currentPieceLocation;
          availableMovesContainer = container;
          break;
        }
      }
    }
  }

  * kingChecked = !boardIsValid;

  if (availableMovesContainer.isEmpty()) {
    // Literally NO PIECE can move
    containerOfAvailableMoves.clear();
    return false;
  }
  else {
    containerOfAvailableMoves = availableMovesContainer;
    return true;
  }
  return false;
}

///
/// \brief MoveMapper::associatedGameBoard is an accessor method to the Board pointer currently assiated with this instance of MoveMapper
/// \return a pointer to the associated Board.
///
Board* MoveMapper::associatedGameBoard() const
{
  return _theGameBoard;
}

///
/// \brief MoveMapper::associateGameBoard is a mutator method to manipulate the the associate game Board pointer.
/// \param associatedGameBoard is the new pointer to store as the associated game Board.
///
void MoveMapper::associateGameBoard(Board* associatedGameBoard)
{
  _theGameBoard = associatedGameBoard;
}
