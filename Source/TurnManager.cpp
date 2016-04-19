#include "TurnManager.h"

#include <QSharedPointer>
#include <QApplication>

QSharedPointer<Player> TurnManager::_currentPlayer = QSharedPointer<Player>();
AppFilter* TurnManager::_filter = new AppFilter();

///
/// TurnManager::TurnManager
/// \param parent
///
TurnManager::TurnManager(QObject* parent) : QObject(parent)
{

}

///
/// TurnManager::currentPlayer
/// \return
///
QSharedPointer<Player>& TurnManager::currentPlayer()
{
  return _currentPlayer;
}

///
/// TurnManager::switchPlayers
/// \param toWhichPlayer
///
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

///
/// TurnManager::setCurrentPlayer
/// \param currentPlayer
///
void TurnManager::setCurrentPlayer(const QSharedPointer<Player>& currentPlayer)
{
  _currentPlayer = currentPlayer;
}

///
/// TurnManager::~TurnManager
///
TurnManager::~TurnManager()
{

}

