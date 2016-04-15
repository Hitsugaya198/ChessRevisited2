#ifndef BOARD_H
#define BOARD_H

#include "Pieces.h"
#include "UserIdentity.h"
#include "MoveRules.h"

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
  typedef QSet < definedPieceType > piecesSetType;
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

  void updatePieceMap(Cell* from, Cell* to, boardStateMapType& boardStateMap, piecesSetType& capturedPiecesContainer);

  Cell* getCell(int row, int column) const;
  Cell* getCell(boardCoordinateType position) const;

  void resetBoard(bool forTheFirstTime, bool styleOnly);

  boardCoordinatesType getPath(boardCoordinateType pointA,
                               boardCoordinateType pointB);

  void mapMoves(MoveRules::movementType rules, definedPieceType piece, boardCoordinatesType& container, boardCoordinateType location);

  boardCoordinatesType findPieces(definedPieceType piece, boardStateMapType& boardStateToSearch);
  bool evaluateBoardState(boardStateMapType& boardStateToEvaluate);
  static void movePieceStart(Board* _this,
                             Cell* fromCell,
                             Cell* toCell,
                             boardStateMapType& scenario = _workingBoardStateMap,
                             piecesSetType& scenarioPieces = _workingCapturedPieces);
  static void movePieceCompleteMove(Board* _this, boardStateMapType& scenario = _workingBoardStateMap);
  static void movePieceRevertMove(boardStateMapType& scenario = _workingBoardStateMap,
                                  piecesSetType& scenarioPieces = _workingCapturedPieces);

  static piecesSetType& workingCapturedPieces();
  static void setWorkingCapturedPieces(const piecesSetType& workingCapturedPieces);

  static piecesSetType& backedUpCapturedPieces();
  static void setBackedUpCapturedPieces(const piecesSetType& backedUpCapturedPieces);

  static piecesSetType& stagingCapturedPieces();
  static void setStagingCapturedPieces(const piecesSetType& stagingCapturedPieces);

  static boardStateMapType& workingBoardStateMap();
  static void setWorkingBoardStateMap(const boardStateMapType& workingBoardStateMap);

  static boardStateMapType& backedUpBoardStateMap();
  static void setBackedUpBoardStateMap(const boardStateMapType& backedUpBoardStateMap);

  static boardStateMapType& stagingBoardStateMap();
  static void setStagingBoardStateMap(const boardStateMapType& stagingBoardStateMap);

  public slots:
  void clearHighLights();
  void endGame(bool checkMate);

  signals:

private slots:

private:
  Ui::Board* ui;

  static boardStateMapType _workingBoardStateMap;
  static boardStateMapType _backedUpBoardStateMap;
  static boardStateMapType _stagingBoardStateMap;

  static piecesSetType _workingCapturedPieces;
  static piecesSetType _backedUpCapturedPieces;
  static piecesSetType _stagingCapturedPieces;

  boardCoordinateType _locationStart;
  boardCoordinateType _locationEnd;

  void resetBoard(bool styleOnly);
  void uncheckAllCheckedCells();
  void initializeBoardCell(Cell* cell);
  void createStartupMap(boardStateMapType& mapToInitialize);
  bool isMoveLegal(boardCoordinateType moveFrom, boardCoordinateType moveTo, boardCoordinatesType& containerToUse);
  void highLightCoordinates(boardCoordinatesType& set);
  void redrawBoardFromMap(boardStateMapType currentBoardStateMap);
};

#endif // BOARD_H
