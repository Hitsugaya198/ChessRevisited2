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
  /**
   * @brief rowType just an integer used as the row indicator of a coordinate
   */
  typedef int rowType;

  /**
   * @brief columnType just an integer used as the column indicator of a coordinate
   */
  typedef int columnType;

  /**
   * @brief boardCoordinateType combined rowType and columnType into a complex type (QPair)
   */
  typedef QPair < rowType, columnType > boardCoordinateType;

  /**
   * @brief boardCoordinatesType a collection of boardCoordinateType values in a QSet
   */
  typedef QSet < boardCoordinateType > boardCoordinatesType;

  /**
   * @brief definedPieceType combines the attributes of a piece into a single type.
   */
  typedef QPair < Pieces::Identities::eIdentities, Pieces::PieceColors::ePieceColors > definedPieceType;

  /**
   * @brief boardStateMapType the state of a board is mapped as all the playable pieces and their locations
   */
  typedef QMap < boardCoordinateType, definedPieceType > boardStateMapType;

  /**
   * @brief boardStateMapIteratorType is an iterator type for QMap. Useful for reverse-iteration.
   */
  typedef QMapIterator < boardCoordinateType, definedPieceType > boardStateMapIteratorType;

  /**
   * @brief piecesListType a list of pieces, used to document captured pieces
   */
  typedef QList < definedPieceType > piecesListType;
}

/**
 * @brief The Board class represents a chess board.
 * In this class, various properties of a chess board are housed,
 * along with member functions that allow manipulation of the board
 * and the game states that the board will go through during the
 * course of a game of chess.
 */
class Board : public QWidget
{
  Q_OBJECT

public:

  /**
   * @brief The eBoardProperties enum defines the limits and starting positions of the Board.
   */
  enum eBoardProperties {
    eMinRow            = 1, /**< enum value eMinRow represents the lowest row number on the Board. */
    eMaxRow            = 8, /**< enum value eMaxRow represents the highest row number on the Board. */
    eMinColumn         = 1, /**< enum value eMinColumn represents the lowest column number on the Board. */
    eMaxColumn         = 8, /**< enum value eMaxColumn represents the highest column number on the Board. */
    ePawnsTopRow       = 2, /**< enum value ePawnsTopRow represents the default row for pawns at the top of the Board. */
    ePawnsBottomRow    = 7, /**< enum value ePawnsBottomRow represents the default row for pawns at the bottom the Board. */
    eOtherTopRow       = 1, /**< enum value eOtherTopRow represents the default row for non-pawn pieces at the top the Board. */
    eOtherBottomRow    = 8, /**< enum value eOtherBottomRow represents the default row for non-pawn pieces at the bottom the Board. */
    eCastleLeftColumn  = 1, /**< enum value eCastleLeftColumn represents the default column for castles at the left the Board. */
    eCastleRightColumn = 8, /**< enum value eCastleRightColumn represents the default column for castles at the right the Board. */
    eKnightLeftColumn  = 2, /**< enum value eKnightLeftColumn represents the default column for knights at the left the Board. */
    eKnightRightColumn = 7, /**< enum value eKnightRightColumn represents the default column for knights at the right the Board. */
    eBishopLeftColumn  = 3, /**< enum value eBishopLeftColumn represents the default column for bishops at the left the Board. */
    eBishopRightColumn = 6, /**< enum value eBishopRightColumn represents the default column for bishops at the right the Board. */
    eKingColumn        = 4, /**< enum value eKingColumn represents the default column for kings on the Board. */
    eQueenColumn       = 5  /**< enum value eQueenColumn represents the default column for queens on the Board. */
  };

  /**
   * @brief Board is the default constructor. No other, as this is a promoted widget, instantiated by generated code.
   * @param parent is the QObject that will eventually destroy the pointer to Board. Probably a pointer to Chess.
   */
  explicit Board(QWidget* parent = 0);

  /**
   * @brief ~Board the virtual destructor of Board (also calls the QObject destructor).
   */
  virtual ~Board();

  /**
   * @brief updatePieceMap() takes a given state map, and "performs a move" from one Cell to another
   * @param from [in] is the Cell where the piece that will be moved resides in the given state map.
   * @param to [in] is the Cell where the piece will be moved to within the same state map.
   * @param boardStateMap [in,out] the board state map to be used during the move.
   * @param capturedPiecesContainer [in,out] is a list used to keep track of captured pieces.
   */
  void updatePieceMap(Cell* from, Cell* to, boardStateMapType& boardStateMap, piecesListType& capturedPiecesContainer);

  /**
   * @brief getCell() returns a pointer to a Cell on the Board.
   * @param row [in] the row coordinate of the Cell to be returned.
   * @param column [in] the column coordinate of the Cell to be returned.
   * @return a Cell pointer of the Cell at (row,column).
   */
  Cell* getCell(int row, int column) const;

  /**
   * @brief getCell() overloaded version that combines row and column into boardCoordinateType.
   * @param position [in] the boardCoordinate of the Cell to be returned.
   * @return the Cell at position.
   */
  Cell* getCell(boardCoordinateType position) const;

  /**
   * @brief resetBoard() resets the Board, Cells, Containers, and can also connect signals/slots.
   * @param forTheFirstTime [in] determines whether the signals from cells have to be connected.
   * @param styleOnly [in] determines whether a Cell will be styled only, or also be allocated a piece.
   */
  void resetBoard(bool forTheFirstTime, bool styleOnly);

  /**
   * @brief getPath() calculates a direct path between two points on the Board.
   * @param pointA [in] is the start coordinate of the path to be calculated.
   * @param pointB [in] is the end coordinate of the path to be calculated.
   * @param boardStateToSearch [in] is the state to use when calculating the path.
   * @return the se of coordinates that represent the path between pointA and pointB
   */
  boardCoordinatesType getPath(boardCoordinateType pointA,
                               boardCoordinateType pointB,
                               boardStateMapType& boardStateToSearch);

  /**
   * @brief brief mapMoves() produces a map of technically correct moves for a piece based on the rules supplied.
   * @param param rules [in] a list of directions and magnitudes that represents what is legal for piece.
   * @param param piece [in] the piece to be mapped.
   * @param param container [in,out] the container to stored the mapped moves in.
   * @param param location [in] the starting location of the piece in question.
   * @param param stateMapToUse [in] is the map used to determine possible moves when calling isMoveLegal().
   */
  void mapMoves(MoveRules::movementType rules, definedPieceType piece, boardCoordinatesType& container, boardCoordinateType location, boardStateMapType& stateMapToUse);

  /**
   * @brief findPieces() is a replacement for the two findPiece functions.
   * @param piece [in] is the identity and color as definedPieceType of the pieces to find
   * @param boardStateToSearch [in] is the state that will be searched to locate the pieces in question
   * @return the boardCoordinatesType defined as a QSet<boardCoordinateType> containing the search result(s)
   */
  boardCoordinatesType findPieces(definedPieceType piece, boardStateMapType& boardStateToSearch);

  /**
   * @brief evaluateBoardState() determines whether the king of the current player is in danger.
   * @param boardStateToEvaluate [in] is the board state to be evaluated.
   * @return the state "validity" of the board (true = king is not checked, i.e. Board state is valid)
   */
  bool evaluateBoardState(boardStateMapType& boardStateToEvaluate);

  /**
   * @brief movePieceStart() prepares the state management to validate and commit a move.
   * @param _this [in] is the Board instance pointer, needed because movePieceStart is a static function.
   * @param fromCell [in] the Cell to move from.
   * @param toCell [in] the Cell to move to.
   * @param scenario [in,out] the board state map on which the move is to be "staged" for validation.
   * @param scenarioPieces [in,out] the list of captured pieces that accompanies the board state.
   */
  static void movePieceStart(Board* _this,
                             Cell* fromCell,
                             Cell* toCell,
                             boardStateMapType& scenario = _workingBoardStateMap,
                             piecesListType& scenarioPieces = _workingCapturedPieces);

  /**
   * @brief movePieceCompleteMove() completes a move (@see movePieceStart() )
   * @param _this [in] is the Board instance pointer, needed because movePieceStart is a static function.
   * @param scenario [in,out] the board state map on which the move is to be "committed" as final.
   */
  static void movePieceCompleteMove(Board* _this, boardStateMapType& scenario = _workingBoardStateMap);

  /**
   * @brief movePieceRevertMove() reverts a move (@see movePieceStart() )
   * @param scenario [in,out] the board state map on which the move is to be "reverted" post-validation.
   * @param scenarioPieces [in,out] the list of captured pieces that accompanies the board state.
   */
  static void movePieceRevertMove(boardStateMapType& scenario = _workingBoardStateMap,
                                  piecesListType& scenarioPieces = _workingCapturedPieces);

  /**
   * @brief workingCapturedPieces() accessor to the default working container of captured pieces.
   * @return the default working container of captured pieces.
   */
  static piecesListType& workingCapturedPieces();

  /**
   * @brief setWorkingCapturedPieces() mutator for manipulating the default backup container of captured pieces.
   * @param workingCapturedPieces [in] new allocation of the default backup container of captured pieces.
   */
  static void setWorkingCapturedPieces(const piecesListType& workingCapturedPieces);

  /**
   * @brief backedUpCapturedPieces() accessor to the default backup container of captured pieces.
   * @return the default backup container of captured pieces.
   */
  static piecesListType& backedUpCapturedPieces();

  /**
   * @brief setBackedUpCapturedPieces() mutator for manipulating the default backup container of captured pieces.
   * @param backedUpCapturedPieces [in] the new allocation of the default backup container of captured pieces.
   */
  static void setBackedUpCapturedPieces(const piecesListType& backedUpCapturedPieces);

  /**
   * @brief stagingCapturedPieces() accessor to the default staging container of captured pieces.
   * @return the default staging container of captured pieces.
   */
  static piecesListType& stagingCapturedPieces();

  /**
   * @brief setStagingCapturedPieces() mutator for manipulating the default staging container of captured pieces.
   * @param stagingCapturedPieces [in] the new allocation of the default staging container of captured pieces.
   */
  static void setStagingCapturedPieces(const piecesListType& stagingCapturedPieces);

  /**
   * @brief workingBoardStateMap() accessor to the default state map used as a starting point for all current moves.
   * @return the default state map used as a starting point for all current moves.
   */
  static boardStateMapType& workingBoardStateMap();

  /**
   * @brief stagingCapturedPieces() accessor to the default staging container of captured pieces.
   * @return the default staging container of captured pieces.
   */
  static void setWorkingBoardStateMap(const boardStateMapType& workingBoardStateMap);

  /**
   * @brief backedUpBoardStateMap() accessor to the default state map used for backup prior to staging a move.
   * @return the default state map used for backup prior to staging a move.
   */
  static boardStateMapType& backedUpBoardStateMap();

  /**
   * @brief setBackedUpBoardStateMap() mutator to manipulate the default state map used for backup prior to staging a move.
   * @param backedUpBoardStateMap [in] new allocation of the default state map used for backup prior to staging a move.
   */
  static void setBackedUpBoardStateMap(const boardStateMapType& backedUpBoardStateMap);

  /**
   * @brief stagingBoardStateMap() accessor to the default state map used for preparing a move.
   * @return the default state map used for preparing a move.
   */
  static boardStateMapType& stagingBoardStateMap();

  /**
   * @brief setStagingBoardStateMap() mutator to manipulate the default state map used for preparing a move.
   * @param stagingBoardStateMap [in] new allocation of the default state map used for preparing a move
   */
  static void setStagingBoardStateMap(const boardStateMapType& stagingBoardStateMap);

  /**
   * @brief humanPlayer() returns the human Player Object
   * @return a QSharedPointer<Player> reference to the caller.
   */
  QSharedPointer<Player>& humanPlayer();

  /**
   * @brief setHumanPlayer() allocates a Player entity to be used by the human player.
   * @param humanPlayer [in] is a const QSharedPointer<Player> reference as created in Chess.cpp, and shared with the Board.
   */
  void setHumanPlayer(const QSharedPointer<Player>& humanPlayer);

  /**
   * @brief aiPlayer() returns the AI Player Object
   * @return a QSharedPointer<Player> reference to the caller.
   */
  QSharedPointer<Player>& aiPlayer();

  /**
   * @brief setAiPlayer() allocates a Player entity to be used for the AI.
   * @param aiPlayer [in] is a const QSharedPointer<Player> reference as created in Chess.cpp, and shared with the Board.
   */
  void setAiPlayer(const QSharedPointer<Player>& aiPlayer);

  /**
   * @brief isTheTargetWithinRange() determines if any piece of the opposing side is able to attack a given piece.
   * @param colorThatIsToBeAttacked [in] the color (black or white) of the piece to be attacked.
   * @param identityThatIsToBeAttacked [in] the identity (pawn, king, knight, etc.) of the piece to be attacked.
   * @param container [in,out] is a container used to output the location of the piece to be attacked.
   * @param boardStateToUse [in] is the state of the Board to use when mapping the attack.
   * @param locationOfAttacker [out] is the location of any piece that can attack the "victim".
   * @param locationOfVictim [out] is the location of the piece that will be attacked.
   * @param pieceWhoWillBeAttacking [out] the definedPieceType value of the attacker.
   * @param pieceWhoWillBeAttacked [out] the definedPieceType value of the victim.
   * @return true if the target can be reached, false if it cannot.
   */
  bool isTheTargetWithinRange(Pieces::PieceColors::ePieceColors colorThatIsToBeAttacked,
                              Pieces::Identities::eIdentities identityThatIsToBeAttacked,
                              boardCoordinatesType& container,
                              boardStateMapType& boardStateToUse,
                              boardCoordinateType& locationOfAttacker,
                              boardCoordinateType& locationOfVictim,
                              definedPieceType& pieceWhoWillBeAttacking,
                              definedPieceType& pieceWhoWillBeAttacked);

  /**
   * @brief locationOfAttacker() returns the board coordinate of the king's attacker, when state evaluation returns as invalid.
   * @return a board coordinate type defined as QPair<int,int> (row,column)
   */
  boardCoordinateType locationOfAttacker() const;

  /**
   * @brief setLocationOfAttacker() setter method for allocating information during state evaluation.
   * @param locationOfAttacker [in] the board coordinate of the king's attacker, set when state evaluation returns as invalid.
   */
  void setLocationOfAttacker(const boardCoordinateType& locationOfAttacker);

  /**
   * @brief locationOfVictim() returns the board coordinate of the king that is checked, when state evaluation returns as invalid.
   * @return a board coordinate type defined as QPair<int,int> (row,column)
   */
  boardCoordinateType locationOfVictim() const;

  /**
   * @brief setLocationOfVictim() setter method for allocating information during state evaluation.
   * @param locationOfVictim [in] the board coordinate of the king that is checked, set when state evaluation returns as invalid.
   */
  void setLocationOfVictim(const boardCoordinateType& locationOfVictim);

  /**
   * @brief pieceWhoWillBeAttacking() provides information about the king's attacker, when state evaluation returns as invalid.
   * @return a defined piece type with information on the piece that will be attacking.
   */
  definedPieceType pieceWhoWillBeAttacking() const;

  /**
   * @brief setPieceWhoWillBeAttacking() setter method for allocating information during state evaluation.
   * @param pieceWhoWillBeAttacking [in] a defined piece type with information on the piece that will be attacking.
   */
  void setPieceWhoWillBeAttacking(const definedPieceType& pieceWhoWillBeAttacking);

  /**
   * @brief pieceWhoWillBeAttacked() provides information about the king that is checked, when state evaluation returns as invalid.
   * @return a defined piece type with information on the piece that will be attacked.
   */
  definedPieceType pieceWhoWillBeAttacked() const;

  /**
   * @brief setPieceWhoWillBeAttacked() setter method for allocating information during state evaluation.
   * @param pieceWhoWillBeAttacked [in] a defined piece type with information on the piece that will be attacked.
   */
  void setPieceWhoWillBeAttacked(const definedPieceType& pieceWhoWillBeAttacked);

  /**
   * @brief uncheckAllCheckedCells() does what its name suggests. It unchecks all the cells on the Board.
   */
  void uncheckAllCheckedCells();

  /**
   * @brief highLightCoordinates() simply does as the name says. It highlights board cells by coordinate.
   * @param set [in] is a QSet<boardCoordinateType> that contains Cell coordinates to highlight.
   */
  void highLightCoordinates(boardCoordinatesType& set);

  /**
   * @brief toggleCell() is a function used by the AI to simulate the clicking of a checkable Cell
   * @param cell [in] is the Cell that the AI wants to "click"
   */
  void toggleCell(Cell* cell);

  /**
   * @brief isMoveLegal() determines whether a mapped move can be allowed.
   * @param moveFrom [in] where the piece is.
   * @param moveTo [in] where the piece wants to go.
   * @param containerToUse [in] the set of coordinates to use in checking the legality of the move.
   * @param stateMapToUse [in] the board state to use for the evaluation.
   * @return true if the move can be allowed, false if not.
   */
  bool isMoveLegal(boardCoordinateType moveFrom,
                   boardCoordinateType moveTo,
                   boardCoordinatesType&
                   containerToUse,
                   boardStateMapType& stateMapToUse);

public slots:
  /**
   * @brief clearHighLights() removes highlighted colouring on all cells across the board
   */
  void clearHighLights();

signals:
  /**
   * @brief moveInitiatedComplete signal is emitted after the first step of a move is completed.
   * @param playerWhoInitiated [in] the player that is making a move.
   */
  void moveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated);

  /**
   * @brief aiMoveCompletionRequired is the signal emitted to make sure the AI completes an initiated move.
   */
  void aiMoveCompletionRequired();

  /**
   * @brief updateCapturedPiecesSignal signal that is emitted to ensure the GUI containers with captured pieces update.
   */
  void updateCapturedPiecesSignal();

private slots:
  /**
   * @brief moveInitiated() prepares the board for an actual move
   * @param fromWhere [in] is a board coordinate (row,column) that the player selected as a starting point.
   */
  void moveInitiated(boardCoordinateType fromWhere);

  /**
   * @brief continueInitiatedMove() is the second leg of a move initiated previously, @see moveInitiated()
   * @param whereTo [in] is the destination cell for the second leg of the move.
   */
  void continueInitiatedMove(boardCoordinateType whereTo);

  /**
   * @brief handleMoveInitiatedComplete() starts a timer of 400ms after a move initiation has completed.
   * @param playerWhoInitiated [in] is either the AI (aiPlayer) or the human participant (humanPlayer)
   */
  void handleMoveInitiatedComplete(QSharedPointer<Player>& playerWhoInitiated);

  /**
   * @brief timerEvent() is called when the 400ms timer fires an event. @see handleMoveInitiatedComplete()
   * @param event [in] details the event that fired.
   */
  void timerEvent(QTimerEvent* event);

private:
  /**
   * @brief ui is the pointer to the graphical part of the Board.
   */
  Ui::Board* ui;

  /**
   * @brief _workingBoardStateMap is the private member that holds the working state of the game board.
   */
  static boardStateMapType _workingBoardStateMap;

  /**
   * @brief _backedUpBoardStateMap is the private member that holds the backed up state of the game board.
   */
  static boardStateMapType _backedUpBoardStateMap;

  /**
   * @brief _stagingBoardStateMap is the private member that holds the state of the game board used for staging moves.
   */
  static boardStateMapType _stagingBoardStateMap;

  /**
   * @brief _workingCapturedPieces is the private member that holds the working state's captured pieces.
   */
  static piecesListType _workingCapturedPieces;

  /**
   * @brief _backedUpCapturedPieces is the private member that holds the backed up state's captured pieces.
   */
  static piecesListType _backedUpCapturedPieces;

  /**
   * @brief _stagingCapturedPieces  is the private member that holds the staging state's captured pieces.
   */
  static piecesListType _stagingCapturedPieces;

  /**
   * @brief _locationStart is the private member that holds the starting position of a move in progress.
   */
  boardCoordinateType _locationStart;

  /**
   * @brief _locationEnd is the private member that holds the end position of a move in progress
   */
  boardCoordinateType _locationEnd;

  /**
   * @brief _containerForMoving is the private member that stores all possible locations an allowed move can result in.
   */
  boardCoordinatesType _containerForMoving;

  /**
   * @brief _humanPlayer stores a shared pointer to the human player object.
   */
  QSharedPointer<Player> _humanPlayer;

  /**
   * @brief _aiPlayer stores a shared pointer to the AI player object.
   */
  QSharedPointer<Player> _aiPlayer;

  /**
   * @brief _locationOfAttacker stores the attacker location for invalid board states.
   */
  boardCoordinateType _locationOfAttacker;

  /**
   * @brief _locationOfVictim stores the victim location for invalid board states.
   */
  boardCoordinateType _locationOfVictim;

  /**
   * @brief _pieceWhoWillBeAttacking stores the attacker information for invalid board states.
   */
  definedPieceType    _pieceWhoWillBeAttacking;

  /**
   * @brief _pieceWhoWillBeAttacked stores the victim information for invalid board states.
   */
  definedPieceType    _pieceWhoWillBeAttacked;

  /**
   * @brief resetBoard() resets all the cells, pieces, and state maps back to their starting states.
   * @param styleOnly [in] determines whether to do reset the style only, or style, states, and containers.
   */
  void resetBoard(bool styleOnly);

  /**
   * @brief initializeBoardCell() assigns a piece to a Cell, determined by the working state map.
   * @param cell [in] the cell that will have a piece allocated to it.
   */
  void initializeBoardCell(Cell* cell);

  /**
   * @brief createStartupMap() creates a map with starting positions of a chess game.
   * @param mapToInitialize [in,out] the map that will be set to "start".
   */
  void createStartupMap(boardStateMapType& mapToInitialize);

  /**
   * @brief redrawBoardFromMap() does what its name suggests.
   * @param currentBoardStateMap [in] the state that will be drawn.
   */
  void redrawBoardFromMap(boardStateMapType currentBoardStateMap);

  /**
   * @brief findPiece() finds a piece defined by color and identity (pawn, knight, etc...)
   * @param colorThatIsToBeFound [in] is the Pieces::PieceColors::ePieceColors value that represents the colour of the piece to be found
   * @param identityThatIsToBeFound [in] is the Pieces::Identities::eIdentities value that represents the identity of the piece to be found
   * @param boardStateToUse [in] the board state to search for the piece in question.
   * @return the coordinate where the piece can be found.
   */
  boardCoordinateType findPiece(Pieces::PieceColors::ePieceColors colorThatIsToBeFound,
                                Pieces::Identities::eIdentities identityThatIsToBeFound,
                                boardStateMapType& boardStateToUse);

  /**
   * @brief findPiece() finds a piece defined by color and identity (pawn, knight, etc...), combined as definedPieceType
   * @param piece [in] a definedPieceType variable that contains the color and identity of the piece to be searched for.
   * @param boardStateToUse [in] the board state to search for the piece in question.
   * @return the coordinate where the piece can be found.
   */
  boardCoordinateType findPiece(definedPieceType piece, boardStateMapType& boardStateToUse);

  /**
   * @brief updateCapturedPieces() emits a signal to the Chess class to update the captured piece containers on the UI.
   */
  void updateCapturedPieces();
};

#endif // BOARD_H
