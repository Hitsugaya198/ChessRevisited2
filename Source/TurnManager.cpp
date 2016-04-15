#include "TurnManager.h"

#include <QSharedPointer>

QSharedPointer<Player> TurnManager::_currentPlayer = QSharedPointer<Player>();

///
/// \brief TurnManager::TurnManager
/// \param parent
///
TurnManager::TurnManager(QObject* parent) : QObject(parent)
{

}

///
/// \brief TurnManager::currentPlayer
/// \return
///
QSharedPointer<Player>& TurnManager::currentPlayer()
{
  return _currentPlayer;
}

///
/// \brief TurnManager::switchPlayers
/// \param toWhichPlayer
///
void TurnManager::switchPlayers(QSharedPointer<Player>& toWhichPlayer)
{
  if (_currentPlayer == 0) {
    _currentPlayer = toWhichPlayer;
  }
  if (_currentPlayer->identity() != toWhichPlayer->identity()) {
    _currentPlayer = toWhichPlayer;
    emit getInstance().turnChanged(_currentPlayer);
  }
}

///
/// \brief TurnManager::setCurrentPlayer
/// \param currentPlayer
///
void TurnManager::setCurrentPlayer(const QSharedPointer<Player>& currentPlayer)
{
  _currentPlayer = currentPlayer;
}

///
/// \brief TurnManager::~TurnManager
///
TurnManager::~TurnManager()
{

}

