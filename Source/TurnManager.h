///
/// \file   TurnManager.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file details the layout and structure of the TurnManager class.
///

#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include "Player.h"
#include "AppFilter.h"
#include "MoveMapper.h"

#include <QObject>

///
/// The TurnManager class
///
class TurnManager : public QObject
{

  Q_OBJECT

public:

  ///
  /// ~TurnManager is the default destructor for objects of type TurnManager.
  ///
  virtual ~TurnManager();

  ///
  /// getInstance returns an instance reference, as the constructor is private.  Singleton Pattern.
  ///
  /// \return an static instance reference to class TurnManager.
  ///
  static TurnManager& getInstance()
  {
    static TurnManager instance;
    return instance;
  }

  ///
  /// currentPlayer is an accessor method for retrieving the Player whose turn to play it currently is.
  ///
  /// \return a shared pointer to an instance of class Player.
  ///
  static QSharedPointer<Player>& currentPlayer();

  ///
  /// switchPlayers effectively ends one Player's turn and notifies the other Player that their turn has started.
  ///
  /// \param toWhichPlayer a shared pointer to an instance of class Player.
  ///
  static void switchPlayers(QSharedPointer<Player>& toWhichPlayer);

signals:

  ///
  /// turnChanged is the signal that notifies Players of a turn-change.
  ///
  /// \param toWhichPlayer a shared pointer to an instance of class Player.
  /// \param containerOfPossibleMoves a container of possible moves for the Player whose turn it is.
  /// \param kingIsChecked a bool indicating whether the current Player's king is checked.
  ///
  void turnChanged(QSharedPointer<Player>& toWhichPlayer, boardCoordinatesType& containerOfPossibleMoves, bool kingIsChecked);

  ///
  /// endGame disables the Board and displayes a message.
  ///
  /// \param checkMate bool indicating if a king was checked when the game ended.
  ///
  void endGame(bool checkMate);

public slots:

private:

  ///
  /// TurnManager is the default constructor for objects of type TurnManager.
  ///
  /// \param parent the QObject that is responsible for destroying TurnManager objects. It can be null.
  ///
  explicit TurnManager(QObject* parent = 0);

  ///
  /// TurnManager declared as private, this copy- by-const-reference constructor is now no longer accessible.
  ///
  TurnManager(TurnManager const&);

  ///
  /// operator =  declared as private, this copy- by-const-reference operator is now no longer accessible.
  ///
  void operator=(TurnManager const&);

  ///
  /// _currentPlayer is the private member that keeps track of whose turn it is to play.
  ///
  static QSharedPointer<Player> _currentPlayer;

  ///
  /// setCurrentPlayer is a private mutator method to manipulate the private member _currentPlayer.
  ///
  /// \param currentPlayer is the new value that _currentPlayer will be set to.
  ///
  void setCurrentPlayer(const QSharedPointer<Player>& currentPlayer);

  ///
  /// _filter is an event filtering object used to ignore all user input when it is the AI's turn to move.
  ///
  static AppFilter* _filter;
};

#endif // TURNMANAGER_H
