#include "Board.h"
#include "ui_Board.h"

#include "Piece.h"
#include "TurnManager.h"

#include <QDebug>
#include <QMessageBox>
#include <QMutexLocker>

// Static member definitions
boardStateMapType Board::_workingBoardStateMap  = boardStateMapType();
boardStateMapType Board::_backedUpBoardStateMap = boardStateMapType();
boardStateMapType Board::_stagingBoardStateMap  = boardStateMapType();

piecesListType Board::_workingCapturedPieces  = piecesListType();
piecesListType Board::_backedUpCapturedPieces = piecesListType();
piecesListType Board::_stagingCapturedPieces  = piecesListType();

///
/// \brief Board::Board
/// \param parent
///
Board::Board(QWidget* parent):
  QWidget(parent),
  ui(new Ui::Board)
{
  ui->setupUi(this);
  resetBoard(true, true);
}

///
/// \brief Board::~Board
///
Board::~Board()
{

}

///
/// \brief Board::updatePieceMap
/// \param from
/// \param to
/// \param boardStateMap
/// \param capturedPiecesContainer
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
/// \brief Board::clearHighLights
///
void Board::clearHighLights()
{
  for (int row = 1; row <= 8; ++row)
    for (int column = 1; column <= 8; ++column) {

      Cell* cell = getCell(row, column);

      if (cell != nullptr) {

        cell->highLightCell(false);
      }
    }
}

///
/// \brief Board::moveInitiated
/// \param fromWhere
///
void Board::moveInitiated(boardCoordinateType fromWhere)
{
  boardCoordinateType locationOfAttacker;
  boardCoordinateType locationOfVictim;
  definedPieceType    pieceWhoWillBeAttacking;
  definedPieceType    pieceWhoWillBeAttacked;

  // validate that there is actually a piece there on the board.
  if (getCell(fromWhere)->assignedPiece()->identity() == Pieces::Identities::eNone ||
      getCell(fromWhere)->assignedPiece()->color() == Pieces::PieceColors::eNone) {
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

    MoveRules::movementType rules = MoveRules::getMovementRules(getCell(fromWhere)->assignedPiece()->identity(), getCell(fromWhere)->assignedPiece()->color());
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
          // Attack the bugger
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
          // Attack the bugger
          _locationStart = locationOfAttacker;
          containerForHighlighting = set;
        }
        else {
          // try to block his path
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
        // Attack the bugger
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
    MoveRules::movementType rules = MoveRules::getMovementRules(getCell(fromWhere)->assignedPiece()->identity(), getCell(fromWhere)->assignedPiece()->color());
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
/// \brief Board::continueInitiatedMove
/// \param whereTo
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
/// \brief Board::handleMoveInitiatedComplete
/// \param playerWhoInitiated
///
void Board::handleMoveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated)
{
  if (playerWhoInitiated->identity() == player2()->identity()) {
    startTimer(400);
  }
}

void Board::timerEvent(QTimerEvent* event)
{
  killTimer(event->timerId());
  emit aiMoveCompletionRequired();
}

///
/// \brief Board::pieceWhoWillBeAttacked
/// \return
///
definedPieceType Board::pieceWhoWillBeAttacked() const
{
  return _pieceWhoWillBeAttacked;
}

///
/// \brief Board::setPieceWhoWillBeAttacked
/// \param pieceWhoWillBeAttacked
///
void Board::setPieceWhoWillBeAttacked(const definedPieceType& pieceWhoWillBeAttacked)
{
  _pieceWhoWillBeAttacked = pieceWhoWillBeAttacked;
}

///
/// \brief Board::pieceWhoWillBeAttacking
/// \return
///
definedPieceType Board::pieceWhoWillBeAttacking() const
{
  return _pieceWhoWillBeAttacking;
}

///
/// \brief Board::setPieceWhoWillBeAttacking
/// \param pieceWhoWillBeAttacking
///
void Board::setPieceWhoWillBeAttacking(const definedPieceType& pieceWhoWillBeAttacking)
{
  _pieceWhoWillBeAttacking = pieceWhoWillBeAttacking;
}

///
/// \brief Board::locationOfVictim
/// \return
///
boardCoordinateType Board::locationOfVictim() const
{
  return _locationOfVictim;
}

///
/// \brief Board::setLocationOfVictim
/// \param locationOfVictim
///
void Board::setLocationOfVictim(const boardCoordinateType& locationOfVictim)
{
  _locationOfVictim = locationOfVictim;
}

///
/// \brief Board::locationOfAttacker
/// \return
///
boardCoordinateType Board::locationOfAttacker() const
{
  return _locationOfAttacker;
}

///
/// \brief Board::setLocationOfAttacker
/// \param locationOfAttacker
///
void Board::setLocationOfAttacker(const boardCoordinateType& locationOfAttacker)
{
  _locationOfAttacker = locationOfAttacker;
}

///
/// \brief Board::player2
/// \return
///
QSharedPointer<Player>& Board::player2()
{
  return _aiPlayer;
}

///
/// \brief Board::setPlayer2
/// \param player2
///
void Board::setPlayer2(const QSharedPointer<Player>& player2)
{
  _aiPlayer = player2;
}

///
/// \brief Board::player1
/// \return
///
QSharedPointer<Player>& Board::player1()
{
  return _humanPlayer;
}

///
/// \brief Board::setPlayer1
/// \param player1
///
void Board::setPlayer1(const QSharedPointer<Player>& player1)
{
  _humanPlayer = player1;
}

///
/// \brief Board::findPiece
/// \param colorThatIsToBeFound
/// \param identityThatIsToBeFound
/// \return
///
boardCoordinateType Board::findPiece(Pieces::PieceColors::ePieceColors colorThatIsToBeFound,
                                     Pieces::Identities::eIdentities identityThatIsToBeFound)
{
  definedPieceType pieceType = definedPieceType(identityThatIsToBeFound, colorThatIsToBeFound);
  return findPiece(pieceType);
}

///
/// \brief Board::findPiece
/// \param piece
/// \return
///
boardCoordinateType Board::findPiece(definedPieceType piece)
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
      definedPieceType currentPiece = _workingBoardStateMap.value(currentCoordinate);

      Pieces::Identities::eIdentities currentPieceIdentity = currentPiece.first;
      Pieces::PieceColors::ePieceColors currentPieceColor = currentPiece.second;

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
/// \brief Board::findPieces
/// \param piece
/// \param boardStateToSearch
/// \return
///
boardCoordinatesType Board::findPieces(definedPieceType piece, boardStateMapType& boardStateToSearch)
{
  boardCoordinatesType retVal;
  if (piece.first == Pieces::Identities::eKing ||
      piece.first == Pieces::Identities::eQueen) { // Only one of each
    boardCoordinateType partOfRetVal = findPiece(piece);
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
      Pieces::PieceColors::ePieceColors currentPieceColor = currentPiece.second;

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
/// \brief Board::highLightCoordinates
/// \param set
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
/// \brief Board::toggleCell
/// \param cell
///
void Board::toggleCell(Cell* cell)
{
  cell->toggle();
}

///
/// \brief Board::getPath
/// \param pointA
/// \param pointB
/// \param boardStateToSearch
/// \return
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
/// \brief Board::evaluateBoardState
/// \param boardStateToEvaluate
/// \return
///
bool Board::evaluateBoardState(boardStateMapType& boardStateToEvaluate)
{
  boardCoordinatesType container;
  Pieces::PieceColors::ePieceColors color;
  if (TurnManager::currentPlayer()->identity() == UserIdentity::eHuman) {
    color = Pieces::PieceColors::eWhite;
  }
  else {
    color = Pieces::PieceColors::eBlack;
  }

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
/// \brief Board::isTheTargetWithinRange
/// \param colorThatIsToBeAttacked
/// \param identityThatIsToBeAttacked
/// \param container
/// \param boardStateToUse
/// \return
///
bool Board::isTheTargetWithinRange(Pieces::PieceColors::ePieceColors colorThatIsToBeAttacked,
                                   Pieces::Identities::eIdentities identityThatIsToBeAttacked,
                                   boardCoordinatesType& container,
                                   boardStateMapType& boardStateToUse,
                                   boardCoordinateType& locationOfAttacker,
                                   boardCoordinateType& locationOfVictim,
                                   definedPieceType& pieceWhoWillBeAttacking,
                                   definedPieceType& pieceWhoWillBeAttacked)
{
  Pieces::PieceColors::ePieceColors attackerColor = Pieces::flipColor(colorThatIsToBeAttacked);
  boardCoordinatesType targetsLocation;
  boardCoordinateType targetLocation = boardCoordinateType(0, 0);

  boardCoordinateType failure = boardCoordinateType(0, 0);
  targetsLocation = findPieces(definedPieceType(identityThatIsToBeAttacked, colorThatIsToBeAttacked), boardStateToUse);
  if (targetsLocation.contains(failure)) {
    return false;
  }

  boardCoordinatesType::iterator boardCoordsIterator;
  bool useIterator = false;
  boardCoordsIterator = targetsLocation.begin();
  useIterator = true;


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
        Pieces::PieceColors::ePieceColors currentPieceColor = currentPiece.second;

        if (currentPieceIdentity == Pieces::Identities::eNone) {
          // No piece on this cell
          continue;
        }
        if (currentPieceColor != attackerColor) {
          // Wrong color for attacker
          continue;
        }
        // It is a piece, it is the right color.  Map it's moves
        MoveRules::movementType rules = MoveRules::getMovementRules(currentPieceIdentity, currentPieceColor);

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
/// \brief Board::movePieceStart
/// \param _this
/// \param fromCell
/// \param toCell
/// \param scenario
/// \param scenarioPieces
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
/// \brief Board::movePieceCompleteMove
/// \param _this
/// \param scenario
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
/// \brief Board::updateCapturedPieces
///
void Board::updateCapturedPieces()
{
  emit updateCapturedPiecesSignal();
}

///
/// \brief Board::movePieceRevertMove
/// \param scenario
/// \param scenarioPieces
///
void Board::movePieceRevertMove(boardStateMapType& scenario, piecesListType& scenarioPieces)
{
  // recover from backup
  scenario = boardStateMapType(_backedUpBoardStateMap);
  scenarioPieces = piecesListType(_backedUpCapturedPieces);
}

///
/// \brief Board::mapMoves
/// \param rules
/// \param piece
/// \param container
/// \param location
///
void Board::mapMoves(MoveRules::movementType rules, definedPieceType piece, boardCoordinatesType& container, boardCoordinateType location, boardStateMapType& stateMapToUse)
{
  bool lessLinearMoveRequired = false;

  if (!container.isEmpty()) { // QSet < QPair < row, column> >
    container.clear();
  }

  int startRow = location.first;
  int startColumn = location.second;

  MoveRules::directionsType moveDirections = rules.first;
  MoveRules::magnitudesType moveMagnitudes = rules.second;
  MoveRules::directionType firstMoveDirections = moveDirections.first;
  //  MoveRules::directionType secondMoveDirections = moveDirections.second;
  MoveRules::magnitudeType firstMagnitude = moveMagnitudes.first;
  MoveRules::magnitudeType secondMagnitude = moveMagnitudes.second;

  // firstMoveDirections are for all pieces
  // secondMoveDirections are only for Knights
  // firstMagnitude is mainly a minimum and secondMagnitude is a maximum
  // except when used for a knight

  if (piece.first == Pieces::Identities::eKnight) {
    lessLinearMoveRequired = true;
  }

  if (piece.first == Pieces::Identities::ePawn) {
    if (piece.second == Pieces::PieceColors::eBlack) {
      if (location.first > 2) {
        secondMagnitude = 1;
      }
      else {
        secondMagnitude = 2;
      }
    }
    else {
      if (location.first < 7) {
        secondMagnitude = 1;
      }
      else {
        secondMagnitude = 2;
      }
    }
  }

  if (!lessLinearMoveRequired) {

    MoveRules::directionType::iterator i = firstMoveDirections.begin();

    while (i != firstMoveDirections.end()) {

      MoveRules::Direction::eDirectionRules temp = *i;
      ++i;

      switch (temp) {
      case MoveRules::Direction::eMayMoveNorth      : {
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
      case MoveRules::Direction::eMayMoveNorthEast  :
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
      case MoveRules::Direction::eMayMoveEast       :
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
      case MoveRules::Direction::eMayMoveSouthEast  :
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
      case MoveRules::Direction::eMayMoveSouth      :
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
      case MoveRules::Direction::eMayMoveSouthWest  :
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
      case MoveRules::Direction::eMayMoveWest       :
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
      case MoveRules::Direction::eMayMoveNorthWest  :
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

    MoveRules::directionType::iterator i = firstMoveDirections.begin();

    while (i != firstMoveDirections.end()) {

      MoveRules::Direction::eDirectionRules temp = *i;
      ++i;

      switch (temp) {
      case MoveRules::Direction::eMayMoveNorthEast  :
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
      case MoveRules::Direction::eMayMoveSouthEast  :
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
      case MoveRules::Direction::eMayMoveSouthWest  :
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
      case MoveRules::Direction::eMayMoveNorthWest  :
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
/// \brief Board::isMoveLegal
/// \param moveFrom
/// \param moveTo
/// \param containerToUse
/// \return
///
bool Board::isMoveLegal(boardCoordinateType moveFrom, boardCoordinateType moveTo, boardCoordinatesType& containerToUse, boardStateMapType& stateMapToUse)
{
  Pieces::PieceColors::ePieceColors fromColor = stateMapToUse.value(moveFrom).second;
  Pieces::PieceColors::ePieceColors toColor = stateMapToUse.value(moveTo).second;

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
        if (fromColor == Pieces::PieceColors::eBlack && row1 > row2) {
          return false;
        }

        if (fromColor == Pieces::PieceColors::eWhite && row1 < row2) {
          return false;
        }

        // Tried to move sideways (attack), but no enemy on that cell.  Will not be allowed.
        if (fromColor == Pieces::PieceColors::eBlack && toColor != Pieces::PieceColors::eWhite) {
          return false;
        }
        else if (fromColor == Pieces::PieceColors::eWhite && toColor != Pieces::PieceColors::eBlack) {
          return false;
        }
        else {
          return true;
        }
      }
      else {  // Straight move, can only attack sideways, so if destination cell contains
        // an enemy piece, thwart it
        if (toColor != Pieces::PieceColors::eNone) {
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
        if (fromColor == Pieces::PieceColors::eBlack && row1 > row2) {
          return false;
        }

        if (fromColor == Pieces::PieceColors::eWhite && row1 < row2) {
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
/// \brief Board::stagingBoardStateMap
/// \return
///
boardStateMapType& Board::stagingBoardStateMap()
{
  return _stagingBoardStateMap;
}

///
/// \brief Board::setStagingBoardStateMap
/// \param stagingBoardStateMap
///
void Board::setStagingBoardStateMap(const boardStateMapType& stagingBoardStateMap)
{
  _stagingBoardStateMap = stagingBoardStateMap;
}

///
/// \brief Board::backedUpBoardStateMap
/// \return
///
boardStateMapType& Board::backedUpBoardStateMap()
{
  return _backedUpBoardStateMap;
}

///
/// \brief Board::setBackedUpBoardStateMap
/// \param backedUpBoardStateMap
///
void Board::setBackedUpBoardStateMap(const boardStateMapType& backedUpBoardStateMap)
{
  _backedUpBoardStateMap = backedUpBoardStateMap;
}

///
/// \brief Board::workingBoardStateMap
/// \return
///
boardStateMapType& Board::workingBoardStateMap()
{
  return _workingBoardStateMap;
}

///
/// \brief Board::setWorkingBoardStateMap
/// \param workingBoardStateMap
///
void Board::setWorkingBoardStateMap(const boardStateMapType& workingBoardStateMap)
{
  _workingBoardStateMap = workingBoardStateMap;
}

///
/// \brief Board::stagingCapturedPieces
/// \return
///
piecesListType& Board::stagingCapturedPieces()
{
  return _stagingCapturedPieces;
}

///
/// \brief Board::setStagingCapturedPieces
/// \param stagingCapturedPieces
///
void Board::setStagingCapturedPieces(const piecesListType& stagingCapturedPieces)
{
  _stagingCapturedPieces = stagingCapturedPieces;
}

///
/// \brief Board::backedUpCapturedPieces
/// \return
///
piecesListType& Board::backedUpCapturedPieces()
{
  return _backedUpCapturedPieces;
}

///
/// \brief Board::setBackedUpCapturedPieces
/// \param backedUpCapturedPieces
///
void Board::setBackedUpCapturedPieces(const piecesListType& backedUpCapturedPieces)
{
  _backedUpCapturedPieces = backedUpCapturedPieces;
}

///
/// \brief Board::workingCapturedPieces
/// \return
///
piecesListType& Board::workingCapturedPieces()
{
  return _workingCapturedPieces;
}

///
/// \brief Board::setWorkingCapturedPieces
/// \param workingCapturedPieces
///
void Board::setWorkingCapturedPieces(const piecesListType& workingCapturedPieces)
{
  _workingCapturedPieces = workingCapturedPieces;
}

///
/// \brief Board::resetBoard
/// \param styleOnly
///
void Board::resetBoard(bool styleOnly)
{
  resetBoard(false, styleOnly);
}

///
/// \brief Board::uncheckAllCheckedCells
///
void Board::uncheckAllCheckedCells()
{
  for (int row = 1; row <= 8; ++row)
    for (int column = 1; column <= 8; ++column) {

      Cell* cell = getCell(row, column);

      if (cell != nullptr) {
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
/// \brief Board::redrawBoardFromMap
/// \param currentBoardStateMap
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
/// \brief Board::resetBoard
/// \param forTheFirstTime
/// \param styleOnly
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

      if (cell != nullptr) {

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
/// \brief Board::getCell
/// \param row
/// \param column
/// \return
///
Cell* Board::getCell(int row, int column) const
{
  QGridLayout* parentLayout = ui->gridLayout;
  Cell* theRealCell = nullptr;

  QLayoutItem* theCell;
  theCell = parentLayout->itemAtPosition(row, column);

  QWidget* closerToTheRealCell;
  closerToTheRealCell = theCell->widget();

  theRealCell = dynamic_cast<Cell*>(closerToTheRealCell);
  return theRealCell;
}

///
/// \brief Board::getCell
/// \param position
/// \return
///
Cell* Board::getCell(boardCoordinateType position) const
{
  return getCell(position.first, position.second);
}

///
/// \brief Board::initializeBoardCell
/// \param cell
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
/// \brief Board::createStartupMap
/// \param mapToInitialize
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
          Pieces::PieceColors::ePieceColors pieceColor;
          if (row == ePawnsTopRow) {
            pieceColor = Pieces::PieceColors::eBlack;
          }
          else {
            pieceColor = Pieces::PieceColors::eWhite;
          }
          mapToInitialize.insert(boardCoordinateType(row, column), definedPieceType(Pieces::Identities::ePawn, pieceColor));
        }

        if (row == eOtherTopRow || row == eOtherBottomRow) { // This is where all the more important enemy pieces start
          Pieces::PieceColors::ePieceColors pieceColor;
          if (row == eOtherTopRow) {
            pieceColor = Pieces::PieceColors::eBlack;
          }
          else {
            pieceColor = Pieces::PieceColors::eWhite;
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
