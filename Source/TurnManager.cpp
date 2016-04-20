///
/// \file   TurnManager.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This cpp file contains the body functionality of the TurnManager class.
///
/// The TurnManager is in essence a state machine that makes sure that users can't
/// play Pieces out of turn, and is also responsible for ending the game.
///

#include "TurnManager.h"

#include <QSharedPointer>
#include <QApplication>

QSharedPointer<Player> TurnManager::_currentPlayer = QSharedPointer<Player>();
AppFilter* TurnManager::_filter = new AppFilter();

TurnManager::TurnManager(QObject* parent) : QObject(parent)
{

}

QSharedPointer<Player>& TurnManager::currentPlayer()
{
  return _currentPlayer;
}

void TurnManager::switchPlayers(QSharedPointer<Player>& toWhichPlayer)
{
  if (_currentPlayer == 0) {
    _currentPlayer = toWhichPlayer;
  }
  if (_currentPlayer->identity() != toWhichPlayer->identity()) {
    _currentPlayer = toWhichPlayer;

    boardCoordinatesType containerOfAvailableMoves;
    boardCoordinateType startLocation;
    bool kingIsChecked = false;

    // Check if the game is over
    if (MoveMapper::getInstance().doesPlayerHaveAvailableMoves(_currentPlayer, containerOfAvailableMoves, &kingIsChecked, startLocation)) {

      if (toWhichPlayer->identity() == UserIdentity::eHuman) {
        QApplication::restoreOverrideCursor();
        QApplication::instance()->removeEventFilter(_filter);
      }
      else {
        QApplication::instance()->installEventFilter(_filter);
        QApplication::setOverrideCursor(Qt::WaitCursor);
      }

      emit getInstance().turnChanged(_currentPlayer, containerOfAvailableMoves, kingIsChecked);
    }
    else
    {
      QApplication::restoreOverrideCursor();
      QApplication::instance()->removeEventFilter(_filter);
      emit getInstance().endGame(kingIsChecked);
    }
  }
}

void TurnManager::setCurrentPlayer(const QSharedPointer<Player>& currentPlayer)
{
  _currentPlayer = currentPlayer;
}

TurnManager::~TurnManager()
{

}
