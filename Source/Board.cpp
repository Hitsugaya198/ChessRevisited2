///
/// \file   Board.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file contains the inner management features of a Chess Board
///
/// In this cpp file is housed all the functions and attributes needed to construct and manage a
/// a chess board in three different stages of play; backed up, current, and proposed states.
///
/// It is made up of Cells that contain pieces, and these are mapped by (x,y) (row,column)
/// coordinate sets.
///
/// The states that are used to keep track of how the game is played are static.
/// The board also contains most of the logic required to restrict and control
/// the movements of pieces across the board.
///
/// The board provides a state-evaluation function, as well as functions to
/// manipulate the states for the purpose of evaluating possible future moves,
/// before committing to make the move.
///
/// The state evaluation only relates to whether or not the current player's king
/// is currently checked (in the state being evaluated).
///
/// If evaluate state finds the state to be invalid, the details are logged as
/// coordinates and pieces that play a role in the checked state of the king.
///

#include "Board.h"
#include "ui_Board.h"

#include "Piece.h"
#include "TurnManager.h"

#include <QDebug>
#include <QMessageBox>

boardStateMapType Board::_workingBoardStateMap  = boardStateMapType();
boardStateMapType Board::_backedUpBoardStateMap = boardStateMapType();
boardStateMapType Board::_stagingBoardStateMap  = boardStateMapType();

piecesListType Board::_workingCapturedPieces  = piecesListType();
piecesListType Board::_backedUpCapturedPieces = piecesListType();
piecesListType Board::_stagingCapturedPieces  = piecesListType();

Board::Board(QWidget* parent):
  QWidget(parent),
  ui(new Ui::Board)
{
  ui->setupUi(this);
  resetBoard(true, true);
}

Board::~Board()
{

}

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

void Board::handleMoveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated)
{
  if (playerWhoInitiated->identity() == aiPlayer()->identity()) {
    startTimer(400);
  }
}

void Board::timerEvent(QTimerEvent* event)
{
  // Immediately kill the timer, as only one action by the AI is required.
  killTimer(event->timerId());
  emit aiMoveCompletionRequired();
}

definedPieceType Board::pieceWhoWillBeAttacked() const
{
  return _pieceWhoWillBeAttacked;
}

void Board::setPieceWhoWillBeAttacked(const definedPieceType& pieceWhoWillBeAttacked)
{
  _pieceWhoWillBeAttacked = pieceWhoWillBeAttacked;
}

definedPieceType Board::pieceWhoWillBeAttacking() const
{
  return _pieceWhoWillBeAttacking;
}

void Board::setPieceWhoWillBeAttacking(const definedPieceType& pieceWhoWillBeAttacking)
{
  _pieceWhoWillBeAttacking = pieceWhoWillBeAttacking;
}

boardCoordinateType Board::locationOfVictim() const
{
  return _locationOfVictim;
}

void Board::setLocationOfVictim(const boardCoordinateType& locationOfVictim)
{
  _locationOfVictim = locationOfVictim;
}

boardCoordinateType Board::locationOfAttacker() const
{
  return _locationOfAttacker;
}

void Board::setLocationOfAttacker(const boardCoordinateType& locationOfAttacker)
{
  _locationOfAttacker = locationOfAttacker;
}

QSharedPointer<Player>& Board::aiPlayer()
{
  return _aiPlayer;
}

void Board::setAiPlayer(const QSharedPointer<Player>& aiPlayer)
{
  _aiPlayer = aiPlayer;
}

QSharedPointer<Player>& Board::humanPlayer()
{
  return _humanPlayer;
}

void Board::setHumanPlayer(const QSharedPointer<Player>& humanPlayer)
{
  _humanPlayer = humanPlayer;
}

boardCoordinateType Board::findPiece(PieceColors::ePieceColors colorThatIsToBeFound,
                                     Pieces::Identities::eIdentities identityThatIsToBeFound,
                                     boardStateMapType& boardStateToUse)
{
  definedPieceType pieceType = definedPieceType(identityThatIsToBeFound, colorThatIsToBeFound);
  return findPiece(pieceType, boardStateToUse);
}

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

void Board::toggleCell(Cell* cell)
{
  cell->toggle();
}

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

void Board::movePieceCompleteMove(Board* _this, boardStateMapType& scenario)
{
  _this->redrawBoardFromMap(scenario);
  _this->clearHighLights();
  _this->uncheckAllCheckedCells();

  // Update the ui containers for captured pieces
  _this->updateCapturedPieces();
}

void Board::updateCapturedPieces()
{
  emit updateCapturedPiecesSignal();
}

void Board::movePieceRevertMove(boardStateMapType& scenario, piecesListType& scenarioPieces)
{
  // recover from backup
  scenario = boardStateMapType(_backedUpBoardStateMap);
  scenarioPieces = piecesListType(_backedUpCapturedPieces);
}

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

boardStateMapType& Board::stagingBoardStateMap()
{
  return _stagingBoardStateMap;
}

void Board::setStagingBoardStateMap(const boardStateMapType& stagingBoardStateMap)
{
  _stagingBoardStateMap = stagingBoardStateMap;
}

boardStateMapType& Board::backedUpBoardStateMap()
{
  return _backedUpBoardStateMap;
}

void Board::setBackedUpBoardStateMap(const boardStateMapType& backedUpBoardStateMap)
{
  _backedUpBoardStateMap = backedUpBoardStateMap;
}

boardStateMapType& Board::workingBoardStateMap()
{
  return _workingBoardStateMap;
}

void Board::setWorkingBoardStateMap(const boardStateMapType& workingBoardStateMap)
{
  _workingBoardStateMap = workingBoardStateMap;
}

piecesListType& Board::stagingCapturedPieces()
{
  return _stagingCapturedPieces;
}

void Board::setStagingCapturedPieces(const piecesListType& stagingCapturedPieces)
{
  _stagingCapturedPieces = stagingCapturedPieces;
}

piecesListType& Board::backedUpCapturedPieces()
{
  return _backedUpCapturedPieces;
}

void Board::setBackedUpCapturedPieces(const piecesListType& backedUpCapturedPieces)
{
  _backedUpCapturedPieces = backedUpCapturedPieces;
}

piecesListType& Board::workingCapturedPieces()
{
  return _workingCapturedPieces;
}

void Board::setWorkingCapturedPieces(const piecesListType& workingCapturedPieces)
{
  _workingCapturedPieces = workingCapturedPieces;
}

void Board::resetBoard(bool styleOnly)
{
  resetBoard(false, styleOnly);
}

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

Cell* Board::getCell(boardCoordinateType position) const
{
  return getCell(position.first, position.second);
}

void Board::initializeBoardCell(Cell* cell)
{
  boardCoordinateType coordinate(cell->position());
  definedPieceType piece = _workingBoardStateMap.value(coordinate);

  // This cell needs a piece
  QSharedPointer<Piece> pieceInstance = QSharedPointer<Piece>(new Piece(piece.first, piece.second));

  cell->assignPiece(pieceInstance);
}

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
