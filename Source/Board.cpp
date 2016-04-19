/**
 * @file   Board.cpp
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the inner management features of a Chess Board
 *
 * In this cpp file is housed all the functions and attributes needed to construct and manage a
 * a chess board in three different stages of play; backed up, current, and proposed states.
 *
 * It is made up of Cells that contain pieces, and these are mapped by (x,y) (row,column)
 * coordinate sets.
 *
 * The states that are used to keep track of how the game is played are static.
 * The board also contains most of the logic required to restrict and control
 * the movements of pieces across the board.
 *
 * The board provides a state-evaluation function, as well as functions to
 * manipulate the states for the purpose of evaluating possible future moves,
 * before committing to make the move.
 *
 * The state evaluation only relates to whether or not the current player's king
 * is currently checked (in the state being evaluated).
 *
 * If evaluate state finds the state to be invalid, the details are logged as
 * coordinates and pieces that play a role in the checked state of the king.
 */

#include "Board.h"
#include "ui_Board.h"

#include "Piece.h"
#include "TurnManager.h"

#include <QDebug>
#include <QMessageBox>

// Static member definitions
boardStateMapType Board::_workingBoardStateMap  = boardStateMapType();
boardStateMapType Board::_backedUpBoardStateMap = boardStateMapType();
boardStateMapType Board::_stagingBoardStateMap  = boardStateMapType();

piecesListType Board::_workingCapturedPieces  = piecesListType();
piecesListType Board::_backedUpCapturedPieces = piecesListType();
piecesListType Board::_stagingCapturedPieces  = piecesListType();

///
/// \brief Board::Board() is the constructor for class Board
/// \param parent is the default parameter for any class based on QObject, like Board is.
///
/// Qt Framework uses the concept of parenting to ensure raw pointer types
/// are destroyed when their parents destroy.  Also, with some GUI classes,
/// parenting automatically embeds or positions the child element inside,
/// or on top of the parent.
///
Board::Board(QWidget* parent):
  QWidget(parent),
  ui(new Ui::Board)
{
  ui->setupUi(this);
  resetBoard(true, true);
}

///
/// \brief Board::~Board() is the destructor for class Board.
///
Board::~Board()
{

}

///
/// \brief Board::updatePieceMap() takes a given state map, and "performs a move" from one Cell to another
/// \param from [in] is the Cell where the piece that will be moved resides in the given state map.
/// \param to [in] is the Cell where the piece will be moved to within the same state map.
/// \param boardStateMap [in,out] the board state map to be used during the move.
/// \param capturedPiecesContainer [in,out] is a list used to keep track of captured pieces.
///
void Board::updatePieceMap(Cell* from, Cell* to, boardStateMapType& boardStateMap, piecesListType& capturedPiecesContainer)
{
  boardCoordinateType fromCoords = boardCoordinateType(from->row(), from->column());
  definedPieceType fromType = definedPieceType(from->assignedPiece()->identity(), from->assignedPiece()->color());

  boardCoordinateType toCoords = boardCoordinateType(to->row(), to->column());
  definedPieceType toType = definedPieceType(to->assignedPiece()->identity(), to->assignedPiece()->color());

  // check if this is an attack
  if (toType.first != Pieces::Identities::eNone) {
    // It's an attack
    // Destination piece is now considered to be captured
    capturedPiecesContainer.append(toType);
    boardStateMap.remove(toCoords);
  }

  Q_ASSERT_X(boardStateMap.contains(fromCoords), "updatePieceMap", "From-position not found in map!");

  boardStateMap.take(fromCoords); // takes the piece from its old location
  boardStateMap.insert(toCoords, fromType); // puts it in its new location
}

///
/// \brief Board::clearHighLights() removes highlighted colouring on all cells across the board
///
void Board::clearHighLights()
{
  for (int row = 1; row <= 8; ++row)
    for (int column = 1; column <= 8; ++column) {

      Cell* cell = getCell(row, column);

      if (cell != NULL) {

        cell->highLightCell(false);
      }
    }
}

///
/// \brief Board::moveInitiated() prepares the board for an actual move
/// \param fromWhere [in] is a board coordinate (row,column) that the player selected as a starting point.
///
void Board::moveInitiated(boardCoordinateType fromWhere)
{
  boardCoordinateType locationOfAttacker;
  boardCoordinateType locationOfVictim;
  definedPieceType    pieceWhoWillBeAttacking;
  definedPieceType    pieceWhoWillBeAttacked;

  // validate that there is actually a piece there on the board.
  if (getCell(fromWhere)->assignedPiece()->identity() == Pieces::Identities::eNone ||
      getCell(fromWhere)->assignedPiece()->color() == PieceColors::eNone) {
    getCell(fromWhere)->setChecked(false);
    getCell(fromWhere)->highLightCell(false);
    return;
  }

  boardCoordinatesType containerForHighlighting = boardCoordinatesType();

  // If we reach this point, it is a real piece, and it is the right color for the user trying to move it.
  // Let's evaluate the current boardState

  bool boardIsValid = evaluateBoardState(_workingBoardStateMap);

  // This would imply the King of the current player is checked, so valid moves
  if (!boardIsValid) {
    // see if any potential moves can bring the board into a legal state (uncheck the king)
    // first check if the selected cell can attack the offender

    movementType rules = MoveRules::getMovementRules(getCell(fromWhere)->assignedPiece()->identity(), getCell(fromWhere)->assignedPiece()->color());
    definedPieceType currentPiece = definedPieceType(getCell(fromWhere)->assignedPiece()->identity(), getCell(fromWhere)->assignedPiece()->color());
    boardCoordinatesType container = boardCoordinatesType();

    // Map it's possible moves.  mapMoves() only returns "legal" moves
    mapMoves(rules, currentPiece, container, fromWhere, _workingBoardStateMap);

    if (getCell(fromWhere)->assignedPiece()->identity() == Pieces::Identities::eKing) {
      if (!container.isEmpty()) {
        boardCoordinatesType set;
        bool canItBeAttacked = isTheTargetWithinRange(_pieceWhoWillBeAttacking.second,
                                                      _pieceWhoWillBeAttacking.first,
                                                      set,
                                                      _workingBoardStateMap,
                                                      locationOfAttacker,
                                                      locationOfVictim,
                                                      pieceWhoWillBeAttacking,
                                                      pieceWhoWillBeAttacked);
        if (canItBeAttacked) {
          // Attack it
          _locationStart = locationOfAttacker;
          containerForHighlighting = set;
        }
        else {
          // Run away
          boardCoordinatesType set = getPath(_locationOfVictim, _locationOfAttacker, _workingBoardStateMap);
          bool pathCanBePotentiallyAvoided = !set.isEmpty();
          if (pathCanBePotentiallyAvoided) {
            boardCoordinatesType possibleMoves = container.subtract(set);
            if (!possibleMoves.isEmpty()) {
              // record the starting-cell, highlight the outcomes
              _locationStart = fromWhere;
              containerForHighlighting = possibleMoves;
            }
            else {
              // Selected piece has no available moves.
            }
          }
        }
      }
    }
    else if (getCell(fromWhere)->assignedPiece()->identity() == Pieces::Identities::eKnight) {
      // Knight's path is L-shaped, not straight horizontal/vertical/diagonal like the other pieces
      // It is a piece, it is the right color.  Map it's move-rules

      if (!container.isEmpty()) {
        boardCoordinatesType set;
        bool canItBeAttacked = isTheTargetWithinRange(_pieceWhoWillBeAttacking.second,
                                                      _pieceWhoWillBeAttacking.first,
                                                      set,
                                                      _workingBoardStateMap,
                                                      locationOfAttacker,
                                                      locationOfVictim,
                                                      pieceWhoWillBeAttacking,
                                                      pieceWhoWillBeAttacked);
        if (canItBeAttacked) {
          // Attack it
          _locationStart = locationOfAttacker;
          containerForHighlighting = set;
        }
        else {
          // Try to block its path
          boardCoordinatesType set = getPath(_locationOfVictim, _locationOfAttacker, _workingBoardStateMap);
          bool pathCanBePotentiallyBlocked = !set.isEmpty();
          if (pathCanBePotentiallyBlocked) {
            boardCoordinatesType possibleMoves = set.intersect(container);
            if (!possibleMoves.isEmpty()) {
              // record the starting-cell, highlight the outcomes
              _locationStart = fromWhere;
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
      boardCoordinatesType set = getPath(fromWhere, _locationOfAttacker, _workingBoardStateMap);

      bool canItBeAttacked = !set.isEmpty();
      // If we can attack it directly, there will be a path to it.
      if (canItBeAttacked) {
        // Attack it
        _locationStart = fromWhere;
        containerForHighlighting = set;
      }
      else { // can we block its path?
        set = getPath(_locationOfVictim, _locationOfAttacker, _workingBoardStateMap);
        bool pathCanBePotentiallyBlocked = !set.isEmpty();
        if (pathCanBePotentiallyBlocked) {
          boardCoordinatesType possibleMoves = set.intersect(container);
          if (!possibleMoves.isEmpty()) {
            // record the starting-cell, highlight the outcomes
            _locationStart = fromWhere;
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
    // Board is in a valid state (your king is not in danger)
    movementType rules = MoveRules::getMovementRules(getCell(fromWhere)->assignedPiece()->identity(), getCell(fromWhere)->assignedPiece()->color());
    definedPieceType currentPiece = definedPieceType(getCell(fromWhere)->assignedPiece()->identity(), getCell(fromWhere)->assignedPiece()->color());
    boardCoordinatesType container = boardCoordinatesType();

    // Map it's possible moves.  mapMoves() only returns "legal" moves
    mapMoves(rules, currentPiece, container, fromWhere, _workingBoardStateMap);


    if (!container.isEmpty()) {
      // One last check to see if any of the proposed moves will in fact result in an invalid board state.
      boardCoordinatesType::iterator containerIterator = container.begin();
      while (containerIterator != container.end()) {
        boardStateMapType tempState = _workingBoardStateMap;
        piecesListType tempPieces = _workingCapturedPieces;
        boardCoordinateType toWhere = *containerIterator;
        Cell* from = getCell(fromWhere);
        Cell* to = getCell(toWhere);
        movePieceStart(this, from, to, tempState, tempPieces);

        bool boardStillValid = evaluateBoardState(tempState);
        if (!boardStillValid) { // this will check the current player's king
          container.remove(toWhere);
          containerIterator = container.begin();
        }
        else {
          ++containerIterator;
        }
        movePieceRevertMove(tempState, tempPieces);
      }

      if (!container.isEmpty()) {
        _locationStart = fromWhere;
        containerForHighlighting = container;
      }
    }
  }

  if (containerForHighlighting.isEmpty()) {
    clearHighLights();
    uncheckAllCheckedCells();
    return;
  }
  else {
    highLightCoordinates(containerForHighlighting);
    _containerForMoving = containerForHighlighting;
  }

  emit moveInitiatedComplete(TurnManager::getInstance().currentPlayer());
}

///
/// \brief Board::continueInitiatedMove() is the second leg of a move initiated previously, @see moveInitiated()
/// \param whereTo [in] is the destination cell for the second leg of the move.
///
void Board::continueInitiatedMove(boardCoordinateType whereTo)
{
  _locationEnd = whereTo;

  // check if the move will be allowed
  if (_containerForMoving.contains(whereTo)) {
    Cell* whereFrom = getCell(_locationStart);
    Cell* whereTo =   getCell(_locationEnd);
    movePieceStart(this, whereFrom, whereTo);
    movePieceCompleteMove(this);


    if (TurnManager::getInstance().currentPlayer()->identity() == UserIdentity::eHuman) {
      TurnManager::switchPlayers(_aiPlayer);
    }
    else {
      TurnManager::switchPlayers(_humanPlayer);
    }

    if (TurnManager::getInstance().currentPlayer()->identity() == UserIdentity::eHuman) {
      bool boardIsValid = evaluateBoardState(_workingBoardStateMap);
      if (!boardIsValid) {
        QMessageBox::information(0, QString("Check!"), QString("Your King is Checked!"), QMessageBox::Ok);
      }
    }

  }
  else {
    clearHighLights();
    uncheckAllCheckedCells();
  }
}

///
/// \brief Board::handleMoveInitiatedComplete() starts a timer of 400ms after a move initiation has completed.
/// \param playerWhoInitiated [in] is either the AI (aiPlayer) or the human participant (humanPlayer)
///
/// A human would normally complete both steps of a movement without being prompted to do so.
/// The AI however is not quite human, and requires some encouragement, hence the timer and timerEvent.
///
void Board::handleMoveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated)
{
  if (playerWhoInitiated->identity() == aiPlayer()->identity()) {
    startTimer(400);
  }
}

///
/// \brief Board::timerEvent() is called when the 400ms timer fires an event. @see handleMoveInitiatedComplete()
/// \param event [in] details the event that fired.
///
void Board::timerEvent(QTimerEvent* event)
{
  // Immediately kill the timer, as only one action by the AI is required.
  killTimer(event->timerId());
  emit aiMoveCompletionRequired();
}

///
/// \brief Board::pieceWhoWillBeAttacked() provides information about the king that is checked, when state evaluation returns as invalid.
/// \return a defined piece type with information on the piece that will be attacked.
///
definedPieceType Board::pieceWhoWillBeAttacked() const
{
  return _pieceWhoWillBeAttacked;
}

///
/// \brief Board::setPieceWhoWillBeAttacked() setter method for allocating information during state evaluation.
/// \param pieceWhoWillBeAttacked [in] a defined piece type with information on the piece that will be attacked.
///
void Board::setPieceWhoWillBeAttacked(const definedPieceType& pieceWhoWillBeAttacked)
{
  _pieceWhoWillBeAttacked = pieceWhoWillBeAttacked;
}

///
/// \brief Board::pieceWhoWillBeAttacking() provides information about the king's attacker, when state evaluation returns as invalid.
/// \return a defined piece type with information on the piece that will be attacking.
///
definedPieceType Board::pieceWhoWillBeAttacking() const
{
  return _pieceWhoWillBeAttacking;
}

///
/// \brief Board::setPieceWhoWillBeAttacking() setter method for allocating information during state evaluation.
/// \param pieceWhoWillBeAttacking [in] a defined piece type with information on the piece that will be attacking.
///
void Board::setPieceWhoWillBeAttacking(const definedPieceType& pieceWhoWillBeAttacking)
{
  _pieceWhoWillBeAttacking = pieceWhoWillBeAttacking;
}

///
/// \brief Board::locationOfVictim() returns the board coordinate of the king that is checked, when state evaluation returns as invalid.
/// \return a board coordinate type defined as QPair<int,int> (row,column)
///
boardCoordinateType Board::locationOfVictim() const
{
  return _locationOfVictim;
}

///
/// \brief Board::setLocationOfVictim() setter method for allocating information during state evaluation.
/// \param locationOfVictim [in] the board coordinate of the king that is checked, set when state evaluation returns as invalid.
///
void Board::setLocationOfVictim(const boardCoordinateType& locationOfVictim)
{
  _locationOfVictim = locationOfVictim;
}

///
/// \brief Board::locationOfAttacker() returns the board coordinate of the king's attacker, when state evaluation returns as invalid.
/// \return a board coordinate type defined as QPair<int,int> (row,column)
///
boardCoordinateType Board::locationOfAttacker() const
{
  return _locationOfAttacker;
}

///
/// \brief Board::setLocationOfAttacker() setter method for allocating information during state evaluation.
/// \param locationOfAttacker [in] the board coordinate of the king's attacker, set when state evaluation returns as invalid.
///
void Board::setLocationOfAttacker(const boardCoordinateType& locationOfAttacker)
{
  _locationOfAttacker = locationOfAttacker;
}

///
/// \brief Board::aiPlayer() returns the AI Player Object
/// \return a QSharedPointer<Player> reference to the caller.
/// Players are created in Chess.cpp
///
QSharedPointer<Player>& Board::aiPlayer()
{
  return _aiPlayer;
}

///
/// \brief Board::setAiPlayer() allocates a Player entity to be used for the AI.
/// \param aiPlayer [in] is a const QSharedPointer<Player> reference as created in Chess.cpp, and shared with the Board.
///
void Board::setAiPlayer(const QSharedPointer<Player>& aiPlayer)
{
  _aiPlayer = aiPlayer;
}

///
/// \brief Board::humanPlayer() returns the human Player Object
/// \return a QSharedPointer<Player> reference to the caller.
/// Players are created in Chess.cpp
///
QSharedPointer<Player>& Board::humanPlayer()
{
  return _humanPlayer;
}

///
/// \brief Board::setHumanPlayer() allocates a Player entity to be used by the human player.
/// \param humanPlayer [in] is a const QSharedPointer<Player> reference as created in Chess.cpp, and shared with the Board.
///
void Board::setHumanPlayer(const QSharedPointer<Player>& humanPlayer)
{
  _humanPlayer = humanPlayer;
}

///
/// \brief Board::findPiece() finds a piece defined by color and identity (pawn, knight, etc...)
/// \param colorThatIsToBeFound [in] is the PieceColors::ePieceColors value that represents the colour of the piece to be found
/// \param identityThatIsToBeFound [in] is the Pieces::Identities::eIdentities value that represents the identity of the piece to be found
/// \param boardStateToUse [in] the board state to search for the piece in question.
/// \return the coordinate where the piece can be found.
/// This implementation is a parameter overloaded version of the one that combines identity and color into definedPieceType
///
boardCoordinateType Board::findPiece(PieceColors::ePieceColors colorThatIsToBeFound,
                                     Pieces::Identities::eIdentities identityThatIsToBeFound,
                                     boardStateMapType& boardStateToUse)
{
  definedPieceType pieceType = definedPieceType(identityThatIsToBeFound, colorThatIsToBeFound);
  return findPiece(pieceType, boardStateToUse);
}

///
/// \brief Board::findPiece() finds a piece defined by color and identity (pawn, knight, etc...), combined as definedPieceType
/// \param piece [in] a definedPieceType variable that contains the color and identity of the piece to be searched for.
/// \param boardStateToUse [in] the board state to search for the piece in question.
/// \return the coordinate where the piece can be found.
/// This implementation is called by a parameter overloaded version of the one that takes identity and color separately.
///
boardCoordinateType Board::findPiece(definedPieceType piece, boardStateMapType& boardStateToUse)
{
  boardCoordinateType pieceLocation = boardCoordinateType(0, 0);
  bool foundIt = false;
  // Iterate through the current board map to see if any enemy piece can reach the piece
  // described by color and identity
  for (int row = eMinRow; row <= eMaxRow; ++row) {
    if (foundIt) {
      break;
    }

    for (int column = eMinColumn; column <= eMaxColumn; ++column) {
      boardCoordinateType currentCoordinate = boardCoordinateType(row, column);
      definedPieceType currentPiece = boardStateToUse.value(currentCoordinate);

      Pieces::Identities::eIdentities currentPieceIdentity = currentPiece.first;
      PieceColors::ePieceColors currentPieceColor = currentPiece.second;

      if (currentPieceIdentity == Pieces::Identities::eNone) {
        // No piece on this cell
        continue;
      }
      if (currentPieceColor != piece.second) {
        // Wrong color for piece
        continue;
      }
      if (currentPieceIdentity != piece.first) {
        // Right color, not an empty cell, wrong identity
        continue;
      }

      // Reaching this point we now have the location of the piece.
      pieceLocation = currentCoordinate;
      foundIt = true;
      break;
    }
  }
  return pieceLocation;
}

///
/// \brief Board::findPieces() is a replacement for the two findPiece functions.
/// \param piece [in] is the identity and color as definedPieceType of the pieces to find
/// \param boardStateToSearch [in] is the state that will be searched to locate the pieces in question
/// \return the boardCoordinatesType defined as a QSet<boardCoordinateType> containing the search result(s)
/// The reason for replacing the findPiece functions with this one, was that with the exception of
/// King and Queen, there may be more than just one of each piece definition on the board at
/// any given time.
///
boardCoordinatesType Board::findPieces(definedPieceType piece, boardStateMapType& boardStateToSearch)
{
  boardCoordinatesType retVal;
  if (piece.first == Pieces::Identities::eKing ||
      piece.first == Pieces::Identities::eQueen) { // Only one of each
    boardCoordinateType partOfRetVal = findPiece(piece, boardStateToSearch);
    retVal.insert(partOfRetVal);
    return retVal;
  }

  boardCoordinateType pieceLocation = boardCoordinateType(0, 0);

  // Iterate through the current board map to see if any enemy piece can reach the piece
  // described by color and identity
  for (int row = eMinRow; row <= eMaxRow; ++row) {
    for (int column = eMinColumn; column <= eMaxColumn; ++column) {
      boardCoordinateType currentCoordinate = boardCoordinateType(row, column);
      definedPieceType currentPiece = _workingBoardStateMap.value(currentCoordinate);

      Pieces::Identities::eIdentities currentPieceIdentity = currentPiece.first;
      PieceColors::ePieceColors currentPieceColor = currentPiece.second;

      if (currentPieceIdentity == Pieces::Identities::eNone) {
        // No piece on this cell
        continue;
      }
      if (currentPieceColor != piece.second) {
        // Wrong color for piece
        continue;
      }
      if (currentPieceIdentity != piece.first) {
        // Right color, not an empty cell, wrong identity
        continue;
      }

      // Reaching this point we now have the location of the piece.
      pieceLocation = currentCoordinate;
      retVal.insert(pieceLocation);
    }
  }
  return retVal;
}

///
/// \brief Board::highLightCoordinates() simply does as the name says. It highlights board cells by coordinate.
/// \param set [in] is a QSet<boardCoordinateType> that contains Cell coordinates to highlight.
/// This function is generally used to display the valid moves a selected piece can make.
///
void Board::highLightCoordinates(boardCoordinatesType& set)
{
  boardCoordinatesType::iterator i = set.begin();

  while (i != set.end()) {
    boardCoordinateType coordinate = *i;
    ++i;

    Cell* toCell = getCell(coordinate.first, coordinate.second);

    toCell->highLightCell(true);
  }
}

///
/// \brief Board::toggleCell() is a function used by the AI to simulate the clicking of a checkable Cell
/// \param cell [in] is the Cell that the AI wants to "click"
///
void Board::toggleCell(Cell* cell)
{
  cell->toggle();
}

///
/// \brief Board::getPath() calculates a direct path between two points on the Board.
/// \param pointA [in] is the start coordinate of the path to be calculated.
/// \param pointB [in] is the end coordinate of the path to be calculated.
/// \param boardStateToSearch [in] is the state to use when calculating the path.
/// \return the se of coordinates that represent the path between pointA and pointB
/// The state is needed, as in two different scenarios, pieces on the board may be in different places, and
/// a crucial element to whether a path between pieces exist, is that there may be no other pieces
/// on the path between them.
///
boardCoordinatesType Board::getPath(boardCoordinateType pointA, boardCoordinateType pointB, boardStateMapType& boardStateToSearch)
{
  boardCoordinatesType returnSet;
  int rowA = pointA.first;
  int columnA = pointA.second;

  int rowB = pointB.first;
  int columnB = pointB.second;

  int rowMax = rowA > rowB ? rowA : rowB;
  int columnMax = columnA > columnB ? columnA : columnB;
  int rowMin = rowA < rowB ? rowA : rowB;
  int columnMin = columnA < columnB ? columnA : columnB;

  int stepFactorRows = (rowA < rowB) ? 1 : -1;
  int stepFactorColumns = (columnA < columnB) ? 1 : -1;
  int newColumn = columnA;

  // Make a few practical decisions.
  if (rowMin == rowMax) { // The piece moves horizontally as the row value doesn't change
    if (getCell(pointA)->assignedPiece()->identity() == Pieces::Identities::ePawn) {
      return returnSet;
    }
    // Only need to iterate through the x-axis
    for (int i = columnA + stepFactorColumns; i != columnB + stepFactorColumns; i += stepFactorColumns) {
      boardCoordinateType temp = boardCoordinateType(rowMax, i);
      if (boardStateToSearch.value(temp).first != Pieces::Identities::eNone) {
        // There is a piece between you and your destination.
        if (i != columnB) { // If it is not the destination block, somebody is in your way.
          returnSet = boardCoordinatesType();
          return returnSet;
        }
        else {
          boardCoordinateType validCoordinate = boardCoordinateType(rowMax, i);
          returnSet.insert(validCoordinate);
        }
        // Already checked the source and destination piece colors
        // If the identity of the piece on the destination block is not eNone, the move is legal
      }
      else {
        boardCoordinateType validCoordinate = boardCoordinateType(rowMax, i);
        returnSet.insert(validCoordinate);
      }
    }
    // The for-loop ran it's course, and the return wasn't called.  I call that a success
    return returnSet;
  }

  if (columnMin == columnMax) { // The piece moves horizontally as the y-axis value doesn't change
    if (getCell(pointA)->assignedPiece()->identity() == Pieces::Identities::eBishop) {
      return returnSet;
    }
    // Only need to iterate through the x-axis
    for (int j = rowA + stepFactorRows; j != rowB + stepFactorRows; j += stepFactorRows) {
      boardCoordinateType temp = boardCoordinateType(j, columnMax);
      if (boardStateToSearch.value(temp).first != Pieces::Identities::eNone) {
        // There is a piece between you and your destination.
        if (j != rowB) { // If it is not the destination block, somebody is in your way.
          returnSet = boardCoordinatesType();
          return returnSet;
        }
        else {
          boardCoordinateType validCoordinate = boardCoordinateType(j, columnMax);
          returnSet.insert(validCoordinate);
        }
        // Already checked the source and destination piece colors
        // If the identity of the piece on the destination block is not eNone, the move is legal
      }
      else {
        boardCoordinateType validCoordinate = boardCoordinateType(j, columnMax);
        returnSet.insert(validCoordinate);
      }
    }
    // The for-loop ran it's course, and the return wasn't called.  I call that a success
    return returnSet;
  }

  // If we reach this line, it means the move is diagonal.  Apart from the Knight piece
  // the difference between xMin/xMax and yMin/yMax should be the same, so check that
  if ((rowMax - rowMin) != (columnMax - columnMin)) { // The poo has hitteth the proverbial fan
    returnSet = boardCoordinatesType();
    return returnSet;
  }

  // Special case - target is an adjacent cell
  if (rowA + stepFactorRows == rowB) {
    if (newColumn + stepFactorColumns == columnB) {
      boardCoordinateType validCoordinate = boardCoordinateType(rowB, columnB);
      returnSet.insert(validCoordinate);
      return returnSet;
    }
  }

  for (int row = rowA + stepFactorRows; row != rowB + stepFactorRows; row += stepFactorRows) {
    for (int col = newColumn + stepFactorColumns; col != columnB + stepFactorColumns; col += stepFactorColumns) {
      boardCoordinateType temp = boardCoordinateType(row, col);
      if (boardStateToSearch.value(temp).first != Pieces::Identities::eNone) {
        // There is a piece between you and your destination.
        if (col != columnB && row != rowB) { // If it is not the destination block, somebody is in your way.
          returnSet = boardCoordinatesType();
          return returnSet;
        }
        // Already checked the source and destination piece colors
        // If the identity of the piece on the destination block is not eNone, the move is legal
      }
      else {
        boardCoordinateType validCoordinate = boardCoordinateType(row, col);
        returnSet.insert(validCoordinate);
      }

      newColumn += stepFactorColumns;
      break; // ensure x and y step together
    }
  }
  // The for-loop ran it's course, and the return wasn't called.  I call that a success
  return returnSet;
}

///
/// \brief Board::evaluateBoardState() determines whether the king of the current player is in danger.
/// \param boardStateToEvaluate [in] is the board state to be evaluated.
/// \return the state "validity" of the board (true = king is not checked, i.e. Board state is valid)
///
/// Example usage:
/// @code
///   bool isCurrentStateValid = evaluateBoardState (currentBoardState);
///   if (isCurrentStateValid) {
///     movePieceStart (this, fromCell, toCell, currentBoardState, currentCapturedPiecesList);
///   }
///
///   bool isCurrentStateStillValid  = evaluateBoardState (currentBoardState);
///   if (isCurrentStateStillValid) {
///     movePieceCompleteMove (this, currentBoardState);
///   }
///   else {
///     movePieceRevertMove (currentBoardState, currentCapturedPiecesList);
///   }
/// @endcode
///
bool Board::evaluateBoardState(boardStateMapType& boardStateToEvaluate)
{
  boardCoordinatesType container;
  PieceColors::ePieceColors color;
  color = TurnManager::currentPlayer()->associatedColor();

  boardCoordinateType locationOfAttacker;
  boardCoordinateType locationOfVictim;
  definedPieceType    pieceWhoWillBeAttacking;
  definedPieceType    pieceWhoWillBeAttacked;

  bool kingIsChecked = isTheTargetWithinRange(color, Pieces::Identities::eKing, container, boardStateToEvaluate,
                                              locationOfAttacker, locationOfVictim, pieceWhoWillBeAttacking, pieceWhoWillBeAttacked);


  if (kingIsChecked) {
    _locationOfAttacker      = locationOfAttacker;
    _locationOfVictim        = locationOfVictim;
    _pieceWhoWillBeAttacking = pieceWhoWillBeAttacking;
    _pieceWhoWillBeAttacked  = pieceWhoWillBeAttacked;
    return false;
  }

  return true;
}

///
/// \brief Board::isTheTargetWithinRange() determines if any piece of the opposing side is able to attack a given piece.
/// \param colorThatIsToBeAttacked [in] the color (black or white) of the piece to be attacked.
/// \param identityThatIsToBeAttacked [in] the identity (pawn, king, knight, etc.) of the piece to be attacked.
/// \param container [in,out] is a container used to output the location of the piece to be attacked.
/// \param boardStateToUse [in] is the state of the Board to use when mapping the attack.
/// \param locationOfAttacker [out] is the location of any piece that can attack the "victim".
/// \param locationOfVictim [out] is the location of the piece that will be attacked.
/// \param pieceWhoWillBeAttacking [out] the definedPieceType value of the attacker.
/// \param pieceWhoWillBeAttacked [out] the definedPieceType value of the victim.
/// \return true if the target can be reached, false if it cannot.
///
bool Board::isTheTargetWithinRange(PieceColors::ePieceColors colorThatIsToBeAttacked,
                                   Pieces::Identities::eIdentities identityThatIsToBeAttacked,
                                   boardCoordinatesType& container,
                                   boardStateMapType& boardStateToUse,
                                   boardCoordinateType& locationOfAttacker,
                                   boardCoordinateType& locationOfVictim,
                                   definedPieceType& pieceWhoWillBeAttacking,
                                   definedPieceType& pieceWhoWillBeAttacked)
{
  PieceColors::ePieceColors attackerColor = PieceColors::flipColor(colorThatIsToBeAttacked);
  boardCoordinatesType targetsLocation;
  boardCoordinateType targetLocation = boardCoordinateType(0, 0);

  boardCoordinateType failure = boardCoordinateType(0, 0);
  targetsLocation = findPieces(definedPieceType(identityThatIsToBeAttacked, colorThatIsToBeAttacked), boardStateToUse);
  if (targetsLocation.contains(failure)) {
    return false;
  }

  boardCoordinatesType::iterator boardCoordsIterator;
  boardCoordsIterator = targetsLocation.begin();


  for (int i = 0; i < targetsLocation.size(); ++i) {

    targetLocation = *boardCoordsIterator;
    ++boardCoordsIterator;

    // So now that we know where the target is, iterate through all enemy pieces, map their moves and
    // see if any legal move includes the target's location.
    for (int row = eMinRow; row <= eMaxRow; ++row) {
      for (int column = eMinColumn; column <= eMaxColumn; ++column) {
        boardCoordinateType currentCoordinate = boardCoordinateType(row, column);
        definedPieceType currentPiece = boardStateToUse.value(currentCoordinate);

        Pieces::Identities::eIdentities currentPieceIdentity = currentPiece.first;
        PieceColors::ePieceColors currentPieceColor = currentPiece.second;

        if (currentPieceIdentity == Pieces::Identities::eNone) {
          // No piece on this cell
          continue;
        }
        if (currentPieceColor != attackerColor) {
          // Wrong color for attacker
          continue;
        }
        // It is a piece, it is the right color.  Map it's moves
        movementType rules = MoveRules::getMovementRules(currentPieceIdentity, currentPieceColor);

        mapMoves(rules, currentPiece, container, currentCoordinate, boardStateToUse);
        definedPieceType pieceToAttack = definedPieceType(identityThatIsToBeAttacked,
                                                          colorThatIsToBeAttacked); //_currentBoardStateMap.value(currentCoordinate);

        if (container.contains(targetLocation)) {
          container.clear();
          locationOfAttacker      = currentCoordinate;
          locationOfVictim        = targetLocation;
          pieceWhoWillBeAttacking = currentPiece;
          pieceWhoWillBeAttacked  = pieceToAttack;
          container.insert(locationOfVictim);
          return true;
        }
      }
    }
  }
  container.clear();
  return false;
}

///
/// \brief Board::movePieceStart() prepares the state management to validate and commit a move.
/// \param _this [in] is the Board instance pointer, needed because movePieceStart is a static function.
/// \param fromCell [in] the Cell to move from.
/// \param toCell [in] the Cell to move to.
/// \param scenario [in,out] the board state map on which the move is to be "staged" for validation.
/// \param scenarioPieces [in,out] the list of captured pieces that accompanies the board state.
///
void Board::movePieceStart(Board* _this, Cell* fromCell, Cell* toCell, boardStateMapType& scenario, piecesListType& scenarioPieces)
{
  // back up previous state
  _backedUpBoardStateMap = boardStateMapType(scenario);
  _backedUpCapturedPieces = piecesListType(scenarioPieces);

  // ensure the future starts with the scenario
  _stagingBoardStateMap = boardStateMapType(scenario);
  _stagingCapturedPieces = piecesListType(scenarioPieces);

  // Update the staging map
  _this->updatePieceMap(fromCell, toCell, _stagingBoardStateMap, _stagingCapturedPieces);

  // transfer the staged state into the scenario
  scenario = boardStateMapType(_stagingBoardStateMap);
  scenarioPieces = piecesListType(_stagingCapturedPieces);
}

///
/// \brief Board::movePieceCompleteMove() completes a move (@see movePieceStart() )
/// \param _this [in] is the Board instance pointer, needed because movePieceStart is a static function.
/// \param scenario [in,out] the board state map on which the move is to be "committed" as final.
///
void Board::movePieceCompleteMove(Board* _this, boardStateMapType& scenario)
{
  _this->redrawBoardFromMap(scenario);
  _this->clearHighLights();
  _this->uncheckAllCheckedCells();

  // Update the ui containers for captured pieces
  _this->updateCapturedPieces();
}

///
/// \brief Board::updateCapturedPieces() emits a signal to the Chess class to update the captured piece containers on the UI.
///
void Board::updateCapturedPieces()
{
  emit updateCapturedPiecesSignal();
}

///
/// \brief Board::movePieceRevertMove() reverts a move (@see movePieceStart() )
/// \param scenario [in,out] the board state map on which the move is to be "reverted" post-validation.
/// \param scenarioPieces [in,out] the list of captured pieces that accompanies the board state.
///
void Board::movePieceRevertMove(boardStateMapType& scenario, piecesListType& scenarioPieces)
{
  // recover from backup
  scenario = boardStateMapType(_backedUpBoardStateMap);
  scenarioPieces = piecesListType(_backedUpCapturedPieces);
}

///
/// \brief Board::mapMoves() produces a map of technically correct moves for a piece based on the rules supplied.
/// \param rules [in] a list of directions and magnitudes that represents what is legal for piece.
/// \param piece [in] the piece to be mapped.
/// \param container [in,out] the container to stored the mapped moves in.
/// \param location [in] the starting location of the piece in question.
/// \param stateMapToUse [in] is the map used to determine possible moves when calling isMoveLegal().
///
void Board::mapMoves(movementType rules, definedPieceType piece, boardCoordinatesType& container, boardCoordinateType location, boardStateMapType& stateMapToUse)
{
  bool lessLinearMoveRequired = false;

  if (!container.isEmpty()) { // QSet < QPair < row, column> >
    container.clear();
  }

  int startRow = location.first;
  int startColumn = location.second;

  directionsType moveDirections = rules.first;
  magnitudesType moveMagnitudes = rules.second;
  directionType firstMoveDirections = moveDirections.first;
  //  directionType secondMoveDirections = moveDirections.second;
  magnitudeType firstMagnitude = moveMagnitudes.first;
  magnitudeType secondMagnitude = moveMagnitudes.second;

  // firstMoveDirections are for all pieces
  // secondMoveDirections are only for Knights
  // firstMagnitude is mainly a minimum and secondMagnitude is a maximum
  // except when used for a knight

  if (piece.first == Pieces::Identities::eKnight) {
    lessLinearMoveRequired = true;
  }

  if (piece.first == Pieces::Identities::ePawn) {
    if (piece.second == PieceColors::eBlack) {
      if (location.first > 2) {
        secondMagnitude = Magnitude::eOne;
      }
      else {
        secondMagnitude = Magnitude::eTwo;
      }
    }
    else {
      if (location.first < 7) {
        secondMagnitude = Magnitude::eOne;
      }
      else {
        secondMagnitude = Magnitude::eTwo;
      }
    }
  }

  if (!lessLinearMoveRequired) {

    directionType::iterator i = firstMoveDirections.begin();

    while (i != firstMoveDirections.end()) {

      Direction::eDirectionRules temp = *i;
      ++i;

      switch (temp) {
      case Direction::eMayMoveNorth      : {
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startRow - x >= 1) {
            boardCoordinateType coord = boardCoordinateType(startRow - x, startColumn);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      }
      case Direction::eMayMoveNorthEast  :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startRow - x >= 1 && startColumn + x <= 8) {
            boardCoordinateType coord = boardCoordinateType(startRow - x, startColumn + x);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      case Direction::eMayMoveEast       :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startColumn + x <= 8) {
            boardCoordinateType coord = boardCoordinateType(startRow, startColumn + x);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      case Direction::eMayMoveSouthEast  :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startRow + x <= 8 && startColumn + x <= 8) {
            boardCoordinateType coord = boardCoordinateType(startRow + x, startColumn + x);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      case Direction::eMayMoveSouth      :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startRow + x <= 8) {
            boardCoordinateType coord = boardCoordinateType(startRow + x, startColumn);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      case Direction::eMayMoveSouthWest  :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startRow + x <= 8 && startColumn - x >= 1) {
            boardCoordinateType coord = boardCoordinateType(startRow + x, startColumn - x);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      case Direction::eMayMoveWest       :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startColumn - x >= 1) {
            boardCoordinateType coord = boardCoordinateType(startRow, startColumn - x);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      case Direction::eMayMoveNorthWest  :
        for (int x = firstMagnitude; x <= secondMagnitude; ++x) {
          if (startRow - x >= 1 && startColumn - x >= 1) {
            boardCoordinateType coord = boardCoordinateType(startRow - x, startColumn - x);
            container.insert(coord);
          }
          else {
            break;
          }
        }
        break;
      }
    }
  }
  else { // This is a knight, he doesn't move quite as linear as the other pieces.

    directionType::iterator i = firstMoveDirections.begin();

    while (i != firstMoveDirections.end()) {

      Direction::eDirectionRules temp = *i;
      ++i;

      switch (temp) {
      case Direction::eMayMoveNorthEast  :
        // Knight can move 2 East and 1 North
        if (startRow - 1 >= 1 && startColumn + 2 <= 8) {
          boardCoordinateType coord = boardCoordinateType(startRow - 1, startColumn + 2);
          container.insert(coord);
        }
        // Knight can move 1 East and 2 North
        if (startRow - 2 >= 1 && startColumn + 1 <= 8) {
          boardCoordinateType coord = boardCoordinateType(startRow - 2, startColumn + 1);
          container.insert(coord);
        }
        break;
      case Direction::eMayMoveSouthEast  :
        // Knight can move 2 East and 1 South
        if (startRow + 1 <= 8 && startColumn + 2 <= 8) {
          boardCoordinateType coord = boardCoordinateType(startRow + 1, startColumn + 2);
          container.insert(coord);
        }
        // Knight can move 1 East and 2 South
        if (startRow + 2 <= 8 && startColumn + 1 <= 8) {
          boardCoordinateType coord = boardCoordinateType(startRow + 2, startColumn + 1);
          container.insert(coord);
        }
        break;
      case Direction::eMayMoveSouthWest  :
        // Knight can move 2 West and 1 South
        if (startRow + 1 <= 8 && startColumn - 2 >= 1) {
          boardCoordinateType coord = boardCoordinateType(startRow + 1, startColumn - 2);
          container.insert(coord);
        }
        // Knight can move 1 West and 2 South
        if (startRow + 2 <= 8 && startColumn - 1 >= 1) {
          boardCoordinateType coord = boardCoordinateType(startRow + 2, startColumn - 1);
          container.insert(coord);
        }
        break;
      case Direction::eMayMoveNorthWest  :
        // Knight can move 2 West and 1 North
        if (startRow - 1 >= 1 && startColumn - 2 >= 1) {
          boardCoordinateType coord = boardCoordinateType(startRow - 1, startColumn - 2);
          container.insert(coord);
        }
        // Knight can move 1 West and 2 North
        if (startRow - 2 >= 1 && startColumn - 1 >= 1) {
          boardCoordinateType coord = boardCoordinateType(startRow - 2, startColumn - 1);
          container.insert(coord);
        }
        break;
      }
    }
  }

  // moves mapped, now remove all the impossible ones
  if (!container.isEmpty()) {
    boardCoordinatesType::iterator i = container.begin();

    while (i != container.end()) {
      boardCoordinateType coordinate = *i;
      ++i;

      if (!isMoveLegal(location, coordinate, container, stateMapToUse)) {
        container.remove(coordinate);
        i = container.begin();
      }
    }
  }
}

///
/// \brief Board::isMoveLegal() determines whether a mapped move can be allowed.
/// \param moveFrom [in] where the piece is.
/// \param moveTo [in] where the piece wants to go.
/// \param containerToUse [in] the set of coordinates to use in checking the legality of the move.
/// \param stateMapToUse [in] the board state to use for the evaluation.
/// \return true if the move can be allowed, false if not.
///
bool Board::isMoveLegal(boardCoordinateType moveFrom,
                        boardCoordinateType moveTo,
                        boardCoordinatesType& containerToUse,
                        boardStateMapType& stateMapToUse)
{
  PieceColors::ePieceColors fromColor = stateMapToUse.value(moveFrom).second;
  PieceColors::ePieceColors toColor = stateMapToUse.value(moveTo).second;

  if (containerToUse.isEmpty()) {
    return false; // No possible moves for selected piece.
  }

  boardCoordinateType chosenSpot = moveTo;
  // Is the choice in the scope of possible moves?  Does the destination contain one of my own pieces?
  if (containerToUse.contains(chosenSpot) && fromColor != toColor) {

    // Now we have determined the selected piece can travel to the chosen spot (cell) through
    // it's legal ranges of movement, but, we don't yet know if it can get there (obstructions?)

    int column1 = moveFrom.second;
    int row1 = moveFrom.first;

    int column2 = moveTo.second;
    int row2 = moveTo.first;

    int columnMax = column1 > column2 ? column1 : column2;
    int rowMax = row1 > row2 ? row1 : row2;
    int columnMin = column1 < column2 ? column1 : column2;
    int rowMin = row1 < row2 ? row1 : row2;

    int stepFactorX = (column1 < column2) ? 1 : -1;
    int stepFactorY = (row1 < row2) ? 1 : -1;

    // If the piece in question is a Pawn, and the FROM and TO columns are not the same, there has to be an
    // enemy piece on the destination cell.  Do a quick check and disqualify if needed.
    if (stateMapToUse.value(moveFrom).first == Pieces::Identities::ePawn) {
      if (column1 != column2) {

        // Tried to move horizontally by more than one cell.  Will not be allowed
        if (columnMax - columnMin > 1) {
          return false;
        }

        // Check that the move is 'forward'
        if (fromColor == PieceColors::eBlack && row1 > row2) {
          return false;
        }

        if (fromColor == PieceColors::eWhite && row1 < row2) {
          return false;
        }

        // Tried to move sideways (attack), but no enemy on that cell.  Will not be allowed.
        if (fromColor == PieceColors::eBlack && toColor != PieceColors::eWhite) {
          return false;
        }
        else if (fromColor == PieceColors::eWhite && toColor != PieceColors::eBlack) {
          return false;
        }
        else {
          return true;
        }
      }
      else {  // Straight move, can only attack sideways, so if destination cell contains
        // an enemy piece, thwart it
        if (toColor != PieceColors::eNone) {
          return false;
        }

        // In the event a pawn tries to move 2 spaces forward, but the cell between
        // the pawn and its destination is occupied, it cannot be allowed
        if (rowMax - rowMin > 1) {
          if (getCell(rowMax - 1, columnMax)->assignedPiece()->identity() != Pieces::Identities::eNone) {
            return false;
          }
        }

        // Check that the move is 'forward'
        if (fromColor == PieceColors::eBlack && row1 > row2) {
          return false;
        }

        if (fromColor == PieceColors::eWhite && row1 < row2) {
          return false;
        }

        // If the code execution reaches this point, we can safely assume the pawn is moving
        // in a single column, and that the destination cell does not contain an enemy.
        // Allow it.
        return true;
      }
    }

    // Knights can "jump over" other pieces, so not going to check if anybody is in his way.
    if (stateMapToUse.value(moveFrom).first != Pieces::Identities::eKnight) {

      // Make a few practical decisions.
      if (columnMin == columnMax) { // The piece moves vertically as the x-axis value doesn't change
        // Only need to iterate through the y-axis
        for (int i = rowMin + 1; i <= rowMax; ++i) {
          boardCoordinateType temp = boardCoordinateType(i, columnMax);
          if (stateMapToUse.value(temp).first != Pieces::Identities::eNone) {
            // There is a piece between you and your destination.
            if (i != rowMax) { // If it is not the destination block, somebody is in your way.
              return false;
            }
            // Already checked the source and destination piece colors
            // If the identity of the piece on the destination block is not eNone, the move is legal
          }
        }
        // The for-loop ran it's course, and the return wasn't called.  I call that a success
        return true;
      }

      if (rowMin == rowMax) { // The piece moves horizontally as the y-axis value doesn't change
        // Only need to iterate through the x-axis
        for (int j = columnMin + 1; j <= columnMax; ++j) {
          boardCoordinateType temp = boardCoordinateType(rowMax, j);
          if (stateMapToUse.value(temp).first != Pieces::Identities::eNone) {
            // There is a piece between you and your destination.
            if (j != columnMax) { // If it is not the destination block, somebody is in your way.
              return false;
            }
            // Already checked the source and destination piece colors
            // If the identity of the piece on the destination block is not eNone, the move is legal
          }
        }
        // The for-loop ran it's course, and the return wasn't called.  I call that a success
        return true;
      }

      // If we reach this line, it means the move is diagonal.  Apart from the Knight piece
      // the difference between xMin/xMax and yMin/yMax should be the same, so check that
      if ((columnMax - columnMin) != (rowMax - rowMin)) { // The poo has hitteth the proverbial fan
        return false;
      }

      // Finally, once we reach this line, the check should be easy
      // Few simple checks
      int newY = row1;

      for (int col = column1 + stepFactorX; col != column2 + stepFactorX; col += stepFactorX) {
        for (int row = newY + stepFactorY; row != row2 + stepFactorY; row += stepFactorY) {
          boardCoordinateType temp = boardCoordinateType(row, col);
          if (stateMapToUse.value(temp).first != Pieces::Identities::eNone) {
            // There is a piece between you and your destination.
            if (col != column2 && row != row2) { // If it is not the destination block, somebody is in your way.
              return false;
            }
            // Already checked the source and destination piece colors
            // If the identity of the piece on the destination block is not eNone, the move is legal
          }
          newY += stepFactorY;
          break; // ensure x and y step together
        }
      }
      // The for-loop ran it's course, and the return wasn't called.  I call that a success
      return true;
    }
    else {
      return true;
    }
  }
  else {
    return false;
  }
}

///
/// \brief Board::stagingBoardStateMap() accessor to the default state map used for preparing a move.
/// \return the default state map used for preparing a move.
///
boardStateMapType& Board::stagingBoardStateMap()
{
  return _stagingBoardStateMap;
}

///
/// \brief Board::setStagingBoardStateMap() mutator to manipulate the default state map used for preparing a move.
/// \param stagingBoardStateMap [in] new allocation of the default state map used for preparing a move
///
void Board::setStagingBoardStateMap(const boardStateMapType& stagingBoardStateMap)
{
  _stagingBoardStateMap = stagingBoardStateMap;
}

///
/// \brief Board::backedUpBoardStateMap() accessor to the default state map used for backup prior to staging a move.
/// \return the default state map used for backup prior to staging a move.
///
boardStateMapType& Board::backedUpBoardStateMap()
{
  return _backedUpBoardStateMap;
}

///
/// \brief Board::setBackedUpBoardStateMap() mutator to manipulate the default state map used for backup prior to staging a move.
/// \param backedUpBoardStateMap [in] new allocation of the default state map used for backup prior to staging a move.
///
void Board::setBackedUpBoardStateMap(const boardStateMapType& backedUpBoardStateMap)
{
  _backedUpBoardStateMap = backedUpBoardStateMap;
}

///
/// \brief Board::workingBoardStateMap() accessor to the default state map used as a starting point for all current moves.
/// \return the default state map used as a starting point for all current moves.
///
boardStateMapType& Board::workingBoardStateMap()
{
  return _workingBoardStateMap;
}

///
/// \brief Board::setWorkingBoardStateMap() mutator for manipulating the default state map used as a starting point for all current moves.
/// \param workingBoardStateMap [in] the new allocation of the default state map used as a starting point for all current moves.
///
void Board::setWorkingBoardStateMap(const boardStateMapType& workingBoardStateMap)
{
  _workingBoardStateMap = workingBoardStateMap;
}

///
/// \brief Board::stagingCapturedPieces() accessor to the default staging container of captured pieces.
/// \return the default staging container of captured pieces.
///
piecesListType& Board::stagingCapturedPieces()
{
  return _stagingCapturedPieces;
}

///
/// \brief Board::setStagingCapturedPieces() mutator for manipulating the default staging container of captured pieces.
/// \param stagingCapturedPieces [in] the new allocation of the default staging container of captured pieces.
///
void Board::setStagingCapturedPieces(const piecesListType& stagingCapturedPieces)
{
  _stagingCapturedPieces = stagingCapturedPieces;
}

///
/// \brief Board::backedUpCapturedPieces() accessor to the default backup container of captured pieces.
/// \return the default backup container of captured pieces.
///
piecesListType& Board::backedUpCapturedPieces()
{
  return _backedUpCapturedPieces;
}

///
/// \brief Board::setBackedUpCapturedPieces() mutator for manipulating the default backup container of captured pieces.
/// \param backedUpCapturedPieces [in] the new allocation of the default backup container of captured pieces.
///
void Board::setBackedUpCapturedPieces(const piecesListType& backedUpCapturedPieces)
{
  _backedUpCapturedPieces = backedUpCapturedPieces;
}

///
/// \brief Board::workingCapturedPieces() accessor to the default working container of captured pieces.
/// \return the default working container of captured pieces.
///
piecesListType& Board::workingCapturedPieces()
{
  return _workingCapturedPieces;
}

///
/// \brief Board::setWorkingCapturedPieces() mutator for manipulating the default backup container of captured pieces.
/// \param workingCapturedPieces [in] new allocation of the default backup container of captured pieces.
///
void Board::setWorkingCapturedPieces(const piecesListType& workingCapturedPieces)
{
  _workingCapturedPieces = workingCapturedPieces;
}

///
/// \brief Board::resetBoard() resets all the cells, pieces, and state maps back to their starting states.
/// \param styleOnly [in] determines whether to do reset the style only, or style, states, and containers.
/// This version of resetBoard is a parameter overload for another resetBoard call that can also pass 'true'
/// as a "this is the first time the board is being reset".  This call however always passes a 'false' value.
///
void Board::resetBoard(bool styleOnly)
{
  resetBoard(false, styleOnly);
}

///
/// \brief Board::uncheckAllCheckedCells() does what its name suggests. It unchecks all the cells on the Board.
///
void Board::uncheckAllCheckedCells()
{
  for (int row = 1; row <= 8; ++row)
    for (int column = 1; column <= 8; ++column) {

      Cell* cell = getCell(row, column);

      if (cell != NULL) {
        if (cell->isChecked()) {
          cell->blockSignals(true);
          cell->setChecked(false);
          cell->blockSignals(false);
        }
      }
    }
  Cell::resetCheckedCounter();
}

///
/// \brief Board::redrawBoardFromMap() does what its name suggests.
/// \param currentBoardStateMap [in] the state that will be drawn.
/// After a move completes, the new board state has to be drawn to
/// visually "make the move".
///
void Board::redrawBoardFromMap(boardStateMapType currentBoardStateMap)
{
  // First, clear the board
  for (int row = eMinRow; row <= eMaxRow; ++row) {
    for (int column = eMinColumn; column <= eMaxColumn; ++column) {
      getCell(row, column)->clearAssignedPiece();
      //      getCell(row, column)->highLightCell(false);
    }
  }

  boardStateMapIteratorType i(currentBoardStateMap);

  while (i.hasNext()) {

    i.next();

    boardCoordinateType coordinate = i.key();
    definedPieceType piece = i.value();

    Cell* cell = getCell(coordinate.first, coordinate.second);

    QSharedPointer<Piece> pieceInstance = QSharedPointer<Piece>(new Piece(piece.first, piece.second));
    cell->assignPiece(pieceInstance);

  }
}

///
/// \brief Board::resetBoard() resets the Board, Cells, Containers, and can also connect signals/slots.
/// \param forTheFirstTime [in] determines whether the signals from cells have to be connected.
/// \param styleOnly [in] determines whether a Cell will be styled only, or also be allocated a piece.
///
void Board::resetBoard(bool forTheFirstTime, bool styleOnly)
{
  // Create a startup map for new games
  createStartupMap(_backedUpBoardStateMap);
  createStartupMap(_workingBoardStateMap);
  createStartupMap(_stagingBoardStateMap);

  // Initialize the board.
  for (int row = 1; row <= 8; ++row)
    for (int column = 1; column <= 8; ++column) {

      Cell* cell = getCell(row, column);

      if (cell != NULL) {

        if (forTheFirstTime) {
          // What to connect these cells to?
          connect(cell, SIGNAL(startingANewMove(boardCoordinateType)), this, SLOT(moveInitiated(boardCoordinateType)));
          connect(cell, SIGNAL(completingMove(boardCoordinateType)), this, SLOT(continueInitiatedMove(boardCoordinateType)));
          connect(cell, SIGNAL(nothingToDo()), this, SLOT(clearHighLights()));
          cell->setCoordinate(row, column);
        }


        if (!styleOnly) {
          initializeBoardCell(cell);
        }
      }
    }
  if (forTheFirstTime) {
    connect(this, SIGNAL(moveInitiatedComplete(QSharedPointer<Player>&)), this, SLOT(handleMoveInitiatedComplete(QSharedPointer<Player>&)));
  }
  setEnabled(true);
}

///
/// \brief Board::getCell() returns a pointer to a Cell on the Board.
/// \param row [in] the row coordinate of the Cell to be returned.
/// \param column [in] the column coordinate of the Cell to be returned.
/// \return a Cell pointer of the Cell at (row,column).
///
Cell* Board::getCell(int row, int column) const
{
  QGridLayout* parentLayout = ui->gridLayout;
  Cell* theRealCell = NULL;

  QLayoutItem* theCell;
  theCell = parentLayout->itemAtPosition(row, column);

  QWidget* closerToTheRealCell;
  closerToTheRealCell = theCell->widget();

  theRealCell = dynamic_cast<Cell*>(closerToTheRealCell);
  return theRealCell;
}

///
/// \brief Board::getCell() overloaded version that combines row and column into boardCoordinateType.
/// \param position [in] the boardCoordinate of the Cell to be returned.
/// \return the Cell at position.
///
Cell* Board::getCell(boardCoordinateType position) const
{
  return getCell(position.first, position.second);
}

///
/// \brief Board::initializeBoardCell() assigns a piece to a Cell, determined by the working state map.
/// \param cell [in] the cell that will have a piece allocated to it.
///
void Board::initializeBoardCell(Cell* cell)
{
  boardCoordinateType coordinate(cell->position());
  definedPieceType piece = _workingBoardStateMap.value(coordinate);

  // This cell needs a piece
  QSharedPointer<Piece> pieceInstance = QSharedPointer<Piece>(new Piece(piece.first, piece.second));

  cell->assignPiece(pieceInstance);
}

///
/// \brief Board::createStartupMap() creates a map with starting positions of a chess game.
/// \param mapToInitialize [in,out] the map that will be set to "start".
///
void Board::createStartupMap(boardStateMapType& mapToInitialize)
{
  if (mapToInitialize.isEmpty()) {
    for (rowType row = eMinRow; row <= eMaxRow; ++row)
      for (columnType column = eMinColumn; column <= eMaxColumn; ++column) {
        if (row > ePawnsTopRow && row < ePawnsBottomRow) { // No pieces on the board between rows 2 and 7
          continue;
        }

        if (row == ePawnsTopRow || row == ePawnsBottomRow) { // This will be all pawns.  Assume player always plays with white
          PieceColors::ePieceColors pieceColor;
          if (row == ePawnsTopRow) {
            pieceColor = PieceColors::eBlack;
          }
          else {
            pieceColor = PieceColors::eWhite;
          }
          mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::ePawn, pieceColor));
        }

        if (row == eOtherTopRow || row == eOtherBottomRow) { // This is where all the more important enemy pieces start
          PieceColors::ePieceColors pieceColor;
          if (row == eOtherTopRow) {
            pieceColor = PieceColors::eBlack;
          }
          else {
            pieceColor = PieceColors::eWhite;
          }
          switch (column) {
          case eCastleLeftColumn:
          case eCastleRightColumn:
            mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::eCastle, pieceColor));
            break;
          case eKnightLeftColumn :
          case eKnightRightColumn:
            mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::eKnight, pieceColor));
            break;
          case eBishopLeftColumn :
          case eBishopRightColumn:
            mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::eBishop, pieceColor));
            break;
          case eKingColumn:
            mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::eKing,   pieceColor));
            break;
          case eQueenColumn:
            mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::eQueen,  pieceColor));
            break;
          }
        }
      }
  }
  else {
    mapToInitialize.clear();
    createStartupMap(mapToInitialize);
  }
}
