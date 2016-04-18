#include "MoveMapper.h"

#include "Cell.h"
#include "Piece.h"

MoveMapper::MoveMapper(QObject* parent) : QObject(parent)
{

}

MoveMapper::~MoveMapper()
{

}

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

      MoveRules::movementType rules = MoveRules::getMovementRules(currentPiece.first, currentPiece.second);
      boardCoordinatesType container = boardCoordinatesType();

      // Map it's possible moves.  _theGameBoard->mapMoves() only returns "legal" moves
      _theGameBoard->mapMoves(rules, currentPiece, container, currentPieceLocation, currentStateOfTheGameBoard);

      // If the current piece is the king, and we cannot attack, we must run away
      if (currentPiece.first == Pieces::Identities::eKing) {
        if (!container.isEmpty()) {
          boardCoordinatesType set;
          bool canItBeAttacked = _theGameBoard->isTheTargetWithinRange(Pieces::flipColor(whichPlayer->associatedColor()),
                                                                       _theGameBoard->pieceWhoWillBeAttacking().first,
                                                                       set,
                                                                       currentStateOfTheGameBoard,
                                                                       locationOfAttacker,
                                                                       locationOfVictim,
                                                                       pieceWhoWillBeAttacking,
                                                                       pieceWhoWillBeAttacked);
          if (canItBeAttacked) {
            // Attack the bugger
            locationStart = locationOfAttacker;
//            _locationEnd = locationOfVictim;
            availableMovesContainer = set;
            break;
          }
          else {
            // Run away
            boardCoordinatesType set = _theGameBoard->getPath(_theGameBoard->locationOfVictim(), _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);
            bool pathCanBePotentiallyAvoided = !set.isEmpty();
            if (pathCanBePotentiallyAvoided) {
              boardCoordinatesType possibleMoves = container.subtract(set);
              if (!possibleMoves.isEmpty()) {
                // record the starting-cell, highlight the outcomes
                locationStart = currentPieceLocation;
//                _locationEnd = *(possibleMoves.begin());
                availableMovesContainer = possibleMoves;
                break;
              }
              else {
                // Selected piece has no available moves.
              }
            }
          }
        }
      }
      else if (currentPiece.first == Pieces::Identities::eKnight) {
        // Knight's path is L-shaped, not straight horizontal/vertical/diagonal like the other pieces
        // It is a piece, it is the right color.  Map it's move-rules

        if (!container.isEmpty()) {
          boardCoordinatesType set;
          bool canItBeAttacked = _theGameBoard->isTheTargetWithinRange(Pieces::flipColor(whichPlayer->associatedColor()),
                                                                       _theGameBoard->pieceWhoWillBeAttacking().first,
                                                                       set,
                                                                       currentStateOfTheGameBoard,
                                                                       locationOfAttacker,
                                                                       locationOfVictim,
                                                                       pieceWhoWillBeAttacking,
                                                                       pieceWhoWillBeAttacked);
          if (canItBeAttacked) {
            // Attack the bugger
            locationStart = locationOfAttacker;
//            _locationEnd = locationOfVictim;
            availableMovesContainer = set;
            break;
          }
          else {
            // try to block his path
            boardCoordinatesType set = _theGameBoard->getPath(_theGameBoard->locationOfVictim(), _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);
            bool pathCanBePotentiallyBlocked = !set.isEmpty();
            if (pathCanBePotentiallyBlocked) {
              boardCoordinatesType possibleMoves = set.intersect(container);
              if (!possibleMoves.isEmpty()) {
                // record the starting-cell, highlight the outcomes
                locationStart = currentPieceLocation;
//                _locationEnd = *(possibleMoves.begin());
                availableMovesContainer = possibleMoves;
              }
              else {
                // Selected piece has no available moves.
              }
            }
          }
        }
      }
      else {
        boardCoordinatesType set;
        bool canItBeAttacked = _theGameBoard->isTheTargetWithinRange(Pieces::flipColor(whichPlayer->associatedColor()),
                                                                     _theGameBoard->pieceWhoWillBeAttacking().first,
                                                                     set,
                                                                     currentStateOfTheGameBoard,
                                                                     locationOfAttacker,
                                                                     locationOfVictim,
                                                                     pieceWhoWillBeAttacking,
                                                                     pieceWhoWillBeAttacked);
        if (canItBeAttacked) {
          // Attack the bugger
          locationStart = locationOfAttacker;
//          _locationEnd = locationOfVictim;
          availableMovesContainer = set;
          break;
        }
        else { // can we block its path? The King should NEVER do this
          set = _theGameBoard->getPath(_theGameBoard->locationOfVictim(), _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);
          bool pathCanBePotentiallyBlocked = !set.isEmpty();
          if (pathCanBePotentiallyBlocked) {

            boardCoordinatesType possibleMoves;
            if (currentPiece.first == Pieces::Identities::eKing) {
              possibleMoves = container.subtract(set);
            }
            else {
              possibleMoves = set.intersect(container);
            }

            if (!possibleMoves.isEmpty()) {

              // One last check to see if any of the proposed moves will in fact result in an invalid board state.
              boardCoordinatesType::iterator possibleMovesIterator = possibleMoves.begin();
              while (possibleMovesIterator != possibleMoves.end()) {
                boardStateMapType tempState = currentStateOfTheGameBoard;
                piecesListType tempPieces =  _theGameBoard->workingCapturedPieces();
                boardCoordinateType toWhere = *possibleMovesIterator;
                Cell* from = _theGameBoard->getCell(currentPieceLocation);
                Cell* to = _theGameBoard->getCell(toWhere);
                Board::movePieceStart(_theGameBoard, from, to, tempState, tempPieces);

                bool boardStillValid = _theGameBoard->evaluateBoardState(tempState);
                if (!boardStillValid) { // this will check the current player's king
                  possibleMoves.remove(toWhere);
                  possibleMovesIterator = possibleMoves.begin();
                }
                else {
                  ++possibleMovesIterator;
                }
                Board::movePieceRevertMove(tempState, tempPieces);
              }

              if (!possibleMoves.isEmpty()) {
                locationStart = currentPieceLocation;
                availableMovesContainer = possibleMoves;
                break;
              }
            }
            else {
              // Selected piece has no available moves.
            }
          }
        }
      }
    }
    else {
      // Board is in a valid state (your king is not in danger)
      MoveRules::movementType rules = MoveRules::getMovementRules(currentPiece.first, currentPiece.second);
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
            if (to->assignedPiece()->color() != Pieces::flipColor(whichPlayer->associatedColor())) {
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

  *kingChecked = boardIsValid;

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

Board* MoveMapper::associatedGameBoard() const
{
  return _theGameBoard;
}

void MoveMapper::associateGameBoard(Board* associatedGameBoard)
{
  _theGameBoard = associatedGameBoard;
}
