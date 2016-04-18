#ifndef BOARD_H
#define BOARD_H

#include "Pieces.h"
#include "UserIdentity.h"
#include "MoveRules.h"
#include "Player.h"

#include <QWidget>
#include <QMap>
#include <QPair>
#include <QSet>
#include <QMutex>

class Cell;

namespace Ui
{
  class Board;
}

namespace    //anonymous
{
  typedef int rowType;
  typedef int columnType;
  typedef QPair < rowType, columnType > boardCoordinateType;
  typedef QSet < boardCoordinateType > boardCoordinatesType;
  typedef QPair < Pieces::Identities::eIdentities, Pieces::PieceColors::ePieceColors > definedPieceType;
  typedef QMap < boardCoordinateType, definedPieceType > boardStateMapType;
  typedef QMapIterator < boardCoordinateType, definedPieceType > boardStateMapIteratorType;
  typedef QList < definedPieceType > piecesListType;
}

class Board : public QWidget
{
  Q_OBJECT

public:

  enum eBoardLimits {
    eMinRow = 1,
    eMaxRow = 8,
    eMinColumn = 1,
    eMaxColumn = 8,
    ePawnsTopRow = 2,
    ePawnsBottomRow = 7,
    eOtherTopRow = 1,
    eOtherBottomRow = 8,
    eCastleLeftColumn = 1,
    eCastleRightColumn = 8,
    eKnightLeftColumn = 2,
    eKnightRightColumn = 7,
    eBishopLeftColumn = 3,
    eBishopRightColumn = 6,
    eKingColumn = 4,
    eQueenColumn = 5
  };

  explicit Board(QWidget* parent = 0);
  virtual ~Board();

  void updatePieceMap(Cell* from, Cell* to, boardStateMapType& boardStateMap, piecesListType& capturedPiecesContainer);

  Cell* getCell(int row, int column) const;
  Cell* getCell(boardCoordinateType position) const;

  void resetBoard(bool forTheFirstTime, bool styleOnly);

  boardCoordinatesType getPath(boardCoordinateType pointA,
                               boardCoordinateType pointB,
                               boardStateMapType& boardStateToSearch);

  void mapMoves(MoveRules::movementType rules, definedPieceType piece, boardCoordinatesType& container, boardCoordinateType location, boardStateMapType& staeMapToUse);

  boardCoordinatesType findPieces(definedPieceType piece, boardStateMapType& boardStateToSearch);
  bool evaluateBoardState(boardStateMapType& boardStateToEvaluate);
  static void movePieceStart(Board* _this,
                             Cell* fromCell,
                             Cell* toCell,
                             boardStateMapType& scenario = _workingBoardStateMap,
                             piecesListType& scenarioPieces = _workingCapturedPieces);
  static void movePieceCompleteMove(Board* _this, boardStateMapType& scenario = _workingBoardStateMap);
  static void movePieceRevertMove(boardStateMapType& scenario = _workingBoardStateMap,
                                  piecesListType& scenarioPieces = _workingCapturedPieces);

  static piecesListType& workingCapturedPieces();
  static void setWorkingCapturedPieces(const piecesListType& workingCapturedPieces);

  static piecesListType& backedUpCapturedPieces();
  static void setBackedUpCapturedPieces(const piecesListType& backedUpCapturedPieces);

  static piecesListType& stagingCapturedPieces();
  static void setStagingCapturedPieces(const piecesListType& stagingCapturedPieces);

  static boardStateMapType& workingBoardStateMap();
  static void setWorkingBoardStateMap(const boardStateMapType& workingBoardStateMap);

  static boardStateMapType& backedUpBoardStateMap();
  static void setBackedUpBoardStateMap(const boardStateMapType& backedUpBoardStateMap);

  static boardStateMapType& stagingBoardStateMap();
  static void setStagingBoardStateMap(const boardStateMapType& stagingBoardStateMap);

  QSharedPointer<Player>& player1();
  void setPlayer1(const QSharedPointer<Player>& player1);

  QSharedPointer<Player>& player2();
  void setPlayer2(const QSharedPointer<Player>& player2);

  bool isTheTargetWithinRange(Pieces::PieceColors::ePieceColors colorThatIsToBeAttacked, Pieces::Identities::eIdentities identityThatIsToBeAttacked, boardCoordinatesType& container, boardStateMapType& boardStateToUse, boardCoordinateType& locationOfAttacker, boardCoordinateType& locationOfVictim, definedPieceType& pieceWhoWillBeAttacking, definedPieceType& pieceWhoWillBeAttacked);

  boardCoordinateType locationOfAttacker() const;
  void setLocationOfAttacker(const boardCoordinateType& locationOfAttacker);

  boardCoordinateType locationOfVictim() const;
  void setLocationOfVictim(const boardCoordinateType& locationOfVictim);

  definedPieceType pieceWhoWillBeAttacking() const;
  void setPieceWhoWillBeAttacking(const definedPieceType& pieceWhoWillBeAttacking);

  definedPieceType pieceWhoWillBeAttacked() const;
  void setPieceWhoWillBeAttacked(const definedPieceType& pieceWhoWillBeAttacked);

  void uncheckAllCheckedCells();
  void highLightCoordinates(boardCoordinatesType& set);
  void toggleCell(Cell* cell);
  bool isMoveLegal(boardCoordinateType moveFrom, boardCoordinateType moveTo, boardCoordinatesType& containerToUse, boardStateMapType& stateMapToUse);

public slots:
  void clearHighLights();

signals:
  void moveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated);
  void aiMoveCompletionRequired();
  void updateCapturedPiecesSignal();

private slots:
  void moveInitiated(boardCoordinateType fromWhere);
  void continueInitiatedMove(boardCoordinateType whereTo);
  void handleMoveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated);
  void timerEvent(QTimerEvent* event);

private:
  Ui::Board* ui;
  static boardStateMapType _workingBoardStateMap;
  static boardStateMapType _backedUpBoardStateMap;
  static boardStateMapType _stagingBoardStateMap;

  static piecesListType _workingCapturedPieces;
  static piecesListType _backedUpCapturedPieces;
  static piecesListType _stagingCapturedPieces;

  boardCoordinateType _locationStart;
  boardCoordinateType _locationEnd;

  boardCoordinatesType _containerForMoving;

  QSharedPointer<Player> _humanPlayer;
  QSharedPointer<Player> _aiPlayer;

  boardCoordinateType _locationOfAttacker;
  boardCoordinateType _locationOfVictim;
  definedPieceType    _pieceWhoWillBeAttacking;
  definedPieceType    _pieceWhoWillBeAttacked;

  void resetBoard(bool styleOnly);
  void initializeBoardCell(Cell* cell);
  void createStartupMap(boardStateMapType& mapToInitialize);
  void redrawBoardFromMap(boardStateMapType currentBoardStateMap);

  boardCoordinateType findPiece(Pieces::PieceColors::ePieceColors colorThatIsToBeFound, Pieces::Identities::eIdentities identityThatIsToBeFound, boardStateMapType& boardStateToUse);
  boardCoordinateType findPiece(definedPieceType piece, boardStateMapType& boardStateToUse);
  void updateCapturedPieces();
};

#endif // BOARD_H
