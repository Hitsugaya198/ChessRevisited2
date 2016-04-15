#include "MoveGenerator.h"
#include "TurnManager.h"

///
/// \brief MoveGenerator::MoveGenerator
/// \param parent
///
MoveGenerator::MoveGenerator(QObject* parent) : QObject(parent)
{

}

///
/// \brief MoveGenerator::~MoveGenerator
///
MoveGenerator::~MoveGenerator()
{

}

///
/// \brief MoveGenerator::handleTurnChange
/// \param itIsNowThisPlayersTurn
///
void MoveGenerator::handleTurnChange(QSharedPointer<Player>& itIsNowThisPlayersTurn)
{
  if (itIsNowThisPlayersTurn->identity() == _aiPlayer->identity()) {
    // My move.  For now, move any piece

    // Find a piece that can move, start by retrieving the current board map
    boardStateMapType currentStateOfTheGameBoard = Board::workingBoardStateMap();
    boardCoordinatesType containerForHighlighting = boardCoordinatesType();

    bool boardIsValid = _theGameBoard->evaluateBoardState(currentStateOfTheGameBoard);

    // Loop through all the black pieces
    boardStateMapType::iterator boardIterator = currentStateOfTheGameBoard.begin();
    while (boardIterator != currentStateOfTheGameBoard.end()) {

      // look at the piece...
      boardCoordinateType currentPieceLocation = boardIterator.key();
      definedPieceType    currentPiece = boardIterator.value();
      ++boardIterator;

      if (currentPiece.second != Pieces::PieceColors::eBlack) {
        continue;
      }

      // This would imply the King of the current player is checked, so valid moves
      if (!boardIsValid) {
        // see if any potential moves can bring the board into a legal state (uncheck the king)
        // first check if the selected cell can attack the offender

        MoveRules::movementType rules = MoveRules::getMovementRules(currentPiece.first, currentPiece.second);
        boardCoordinatesType container = boardCoordinatesType();

        // Map it's possible moves.  _theGameBoard->mapMoves() only returns "legal" moves
        _theGameBoard->mapMoves(rules, currentPiece, container, currentPieceLocation);

        if (currentPiece.first == Pieces::Identities::eKnight) {
          // Knight's path is L-shaped, not straight horizontal/vertical/diagonal like the other pieces
          // It is a piece, it is the right color.  Map it's move-rules

          if (!container.isEmpty()) {
            bool canItBeAttacked = container.contains(_theGameBoard->locationOfAttacker());
            if (canItBeAttacked) {
              // Attack the bugger
            }
            else {
              // try to block his path
              boardCoordinatesType set = _theGameBoard->getPath(_theGameBoard->locationOfVictim(), _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);
              bool pathCanBePotentiallyBlocked = !set.isEmpty();
              if (pathCanBePotentiallyBlocked) {
                boardCoordinatesType possibleMoves = set.intersect(container);
                if (!possibleMoves.isEmpty()) {
                  // record the starting-cell, highlight the outcomes
                  _locationStart = currentPieceLocation;
                  containerForHighlighting = possibleMoves;
                }
                else {
                  // Selected piece has no available moves.
                }
              }
            }
          }
        }
        else {
          boardCoordinatesType set = _theGameBoard->getPath(currentPieceLocation, _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);

          bool canItBeAttacked = !set.isEmpty();
          // If we can attack it directly, there will be a path to it.
          if (canItBeAttacked) {
            // One last check to see if any of the proposed moves will in fact result in an invalid board state.
            boardCoordinatesType::iterator setIterator = set.begin();
            while (setIterator != set.end()) {
              boardStateMapType tempState = currentStateOfTheGameBoard;
              piecesSetType tempPieces =  _theGameBoard->workingCapturedPieces();
              boardCoordinateType toWhere = *setIterator;
              Cell* from = _theGameBoard->getCell(currentPieceLocation);
              Cell* to = _theGameBoard->getCell(toWhere);
              Board::movePieceStart(_theGameBoard, from, to, tempState, tempPieces);

              bool boardStillValid = _theGameBoard->evaluateBoardState(tempState);
              if (!boardStillValid) { // this will check the current player's king
                set.remove(toWhere);
                setIterator = set.begin();
              }
              else {
                ++setIterator;
              }
              Board::movePieceRevertMove(tempState, tempPieces);
            }

            if (!set.isEmpty()) {
              _locationStart = currentPieceLocation;
              containerForHighlighting = set;
              break;
            }
          }
          else { // can we block its path?
            set = _theGameBoard->getPath(_theGameBoard->locationOfVictim(), _theGameBoard->locationOfAttacker(), currentStateOfTheGameBoard);
            bool pathCanBePotentiallyBlocked = !set.isEmpty();
            if (pathCanBePotentiallyBlocked) {
              boardCoordinatesType possibleMoves = set.intersect(container);

              if (!possibleMoves.isEmpty()) {

                // One last check to see if any of the proposed moves will in fact result in an invalid board state.
                boardCoordinatesType::iterator possibleMovesIterator = possibleMoves.begin();
                while (possibleMovesIterator != possibleMoves.end()) {
                  boardStateMapType tempState = currentStateOfTheGameBoard;
                  piecesSetType tempPieces =  _theGameBoard->workingCapturedPieces();
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
                  _locationStart = currentPieceLocation;
                  containerForHighlighting = possibleMoves;
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
        _theGameBoard->mapMoves(rules, currentPiece, container, currentPieceLocation);


        if (!container.isEmpty()) {
          // One last check to see if any of the proposed moves will in fact result in an invalid board state.
          boardCoordinatesType::iterator containerIterator = container.begin();
          while (containerIterator != container.end()) {
            boardStateMapType tempState = currentStateOfTheGameBoard;
            piecesSetType tempPieces =  _theGameBoard->workingCapturedPieces();
            boardCoordinateType toWhere = *containerIterator;
            Cell* from = _theGameBoard->getCell(currentPieceLocation);
            Cell* to = _theGameBoard->getCell(toWhere);
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
            _locationStart = currentPieceLocation;
            containerForHighlighting = container;
            break;
          }
        }
      }
    }

    if (containerForHighlighting.isEmpty()) {
      _theGameBoard->clearHighLights();
      _theGameBoard->uncheckAllCheckedCells();
    }
    else {
      _theGameBoard->highLightCoordinates(containerForHighlighting);
      // select the first move and go for it.
      _containerForMoving = containerForHighlighting;
      _theGameBoard->toggleCell(_theGameBoard->getCell(_locationStart));
    }
  }
}

///
/// \brief MoveGenerator::handleMoveCompletionRequired
///
void MoveGenerator::handleMoveCompletionRequired()
{
  if (!_containerForMoving.isEmpty()) {
    _locationEnd = *(_containerForMoving.begin());
    _theGameBoard->toggleCell(_theGameBoard->getCell(_locationEnd));
  }
}

///
/// \brief MoveGenerator::associatedGameBoard
/// \return
///
Board* MoveGenerator::associatedGameBoard() const
{
  return _theGameBoard;
}

///
/// \brief MoveGenerator::associateGameBoard
/// \param theGameBoard
///
void MoveGenerator::associateGameBoard(Board* theGameBoard)
{
  _theGameBoard = theGameBoard;
}

///
/// \brief MoveGenerator::aiPlayer
/// \return
///
QSharedPointer<Player> MoveGenerator::aiPlayer() const
{
  return _aiPlayer;
}

///
/// \brief MoveGenerator::setAiPlayer
/// \param aiPlayer
///
void MoveGenerator::setAiPlayer(const QSharedPointer<Player>& aiPlayer)
{
  _aiPlayer = aiPlayer;
}
