#include "TurnManager.h"

#include <QSharedPointer>

QSharedPointer<Player> TurnManager::_currentPlayer = QSharedPointer<Player>();

TurnManager::TurnManager(QObject *parent) : QObject(parent)
{

}

QSharedPointer<Player>& TurnManager::currentPlayer()
{
  return _currentPlayer;
}

void TurnManager::switchPlayers(QSharedPointer<Player>& toWhichPlayer)
{
  if(_currentPlayer == 0) _currentPlayer = toWhichPlayer;
  if(_currentPlayer->identity() != toWhichPlayer->identity())
  {
    _currentPlayer = toWhichPlayer;
    emit getInstance().turnChanged(_currentPlayer);
  }
}

void TurnManager::setCurrentPlayer(const QSharedPointer<Player>& currentPlayer)
{
  _currentPlayer = currentPlayer;
}


TurnManager::~TurnManager()
{

}

