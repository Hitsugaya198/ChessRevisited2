///
/// \file   MoveGenerator.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains structure definitions and members of the MoveGenerator class.
///

#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Player.h"
#include "Board.h"

#include <QObject>

///
/// The MoveGenerator class is a simple AI class that generates moves for an associated Player instance.
///
class MoveGenerator : public QObject
{

  Q_OBJECT

public:

    ///
    /// MoveGenerator is the default constructor for objects of the MoveGenerator class.
    ///
    /// \param parent is the QObject that will eventually destroy the pointer to MoveGenerator (if not null).
    ///
  explicit MoveGenerator(QObject* parent = 0);

    ///
    /// ~MoveGenerator is the default destructor for objects of type MoveGenerator.
    ///
  virtual ~MoveGenerator();

    ///
    /// aiPlayer is an accessor method to the AI Player associated with this MoveGenerator instance.
    ///
    /// \return the currently associated AI Player for this MoveGenerator instance.
    ///
  QSharedPointer<Player> aiPlayer() const;

  ///
  /// setAiPlayer is a mutator method that associates a Player instance with this MoveGenerator instance.
  ///
  /// \param aiPlayer is the Player instance that will henceforth be associated with this MoveGenerator instance.
  ///
  void setAiPlayer(const QSharedPointer<Player>& aiPlayer);

  ///
  /// associatedGameBoard is an accessor method to the game Board associated with this MoveGenerator instance.
  /// \return the currently associated Board as a pointer.
  ///
  Board* associatedGameBoard() const;

  ///
  /// associateGameBoard is a mutator method that associates a game Board with this MoveGenerator instance.
  ///
  /// \param theGameBoard is the Board pointer that will henceforth be associated to this MoveGenerator instance.
  ///
  void associateGameBoard(Board* associatedGameBoard);

public slots:

  ///
  /// handleTurnChange is the function that gets invoked when it is the AI's turn to move.
  ///
  /// \param itIsNowThisPlayersTurn indicates whose turn it is to move.
  /// \param containerOfPossibleMoves is a container of moves that proves a move can be made.
  /// \param kingIsChecked indicates whether the king of the Player whose turn it is, is checked.
  ///
  void handleTurnChange(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                        boardCoordinatesType& containerOfPossibleMoves,
                        bool kingIsChecked);

  ///
  /// handleMoveCompletionRequired is called after a move has been initiated, in order to complete the move.
  ///
  void handleMoveCompletionRequired();

private:
  ///
  /// _aiPlayer is a shared pointer type that stores an instance of the AI Player created by Chess.
  ///
  QSharedPointer<Player> _aiPlayer;

  ///
  /// _theGameBoard is a raw pointer to the game Board as created by the Ui::Chess class.
  ///
  /// It cannot be a shared or scoped pointer, as the developer has no control over its owner,
  /// and creating shared or scoped pointers of the Board will result in multiple deletion of the
  /// board pointer, and utlimately an application crash on exit.
  ///
  Board* _theGameBoard;

  ///
  /// _locationStart the location where the AI Player will begin its next move.
  ///
  boardCoordinateType _locationStart;

  ///
  /// _locationEnd the location where the AI Player will move to from _locationStart.
  ///
  boardCoordinateType _locationEnd;

  ///
  /// _containerForMoving contains only valid moves at the time the AI Player needs to move, as it can then just grab a move from the container.
  ///
  boardCoordinatesType _containerForMoving;

  ///
  /// _moveStrategy just an integer used in a simple round-robin method of determining how to make the next move.
  ///
  static int _moveStrategy;

  ///
  /// _reverseIterateThroughPieces is a boolean that indicates whether to iterate through moves from the back or front of the containers.
  ///
  /// It randomizes piece and move selection somewhat.
  ///
  static bool _reverseIterateThroughPieces;

  ///
  /// moveTheFirstPieceThatCanMove does as its name suggests.  In some cases, priority is given to moves that can capture enemy pieces.
  ///
  /// \param itIsNowThisPlayersTurn is the Player whose turn it is to play.
  /// \param containerOfPossibleMoves is a container used for keeping track of what is possible.
  /// \param kingIsChecked is an indication of whether moves will be restricted to that which will uncheck the king.
  /// \param priorityForAttack is an indication of whether priority is to capture an enemy piece or not.
  ///
  void moveTheFirstPieceThatCanMove(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                    boardCoordinatesType& containerOfPossibleMoves,
                                    bool kingIsChecked,
                                    bool priorityForAttack = false);

  ///
  /// moveWithPriorityForAttack wrapper to call moveTheFirstPieceThatCanMove with priority given to moves that can capture enemy pieces.
  ///
  /// \param itIsNowThisPlayersTurn is the Player whose turn it is.
  /// \param containerOfPossibleMoves is a container used for keeping track of what is possible.
  /// \param kingIsChecked is an indication of whether moves will be restricted to that which will uncheck the king.
  ///
  void moveWithPriorityForAttack(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                                 boardCoordinatesType& containerOfPossibleMoves,
                                 bool kingIsChecked);

  ///
  /// moveAnyPieceThatCanMove wrapper to call moveTheFirstPieceThatCanMove without priority given to moves that can capture enemy pieces.
  ///
  /// \param itIsNowThisPlayersTurn is the Player whose turn it is.
  /// \param containerOfPossibleMoves is a container used for keeping track of what is possible.
  /// \param kingIsChecked is an indication of whether moves will be restricted to that which will uncheck the king.
  ///
  void moveAnyPieceThatCanMove(QSharedPointer<Player>& itIsNowThisPlayersTurn,
                               boardCoordinatesType& containerOfPossibleMoves,
                               bool kingIsChecked);
};

#endif // MOVEGENERATOR_H
