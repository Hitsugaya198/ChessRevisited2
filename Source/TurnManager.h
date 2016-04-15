#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include "Player.h"

#include <QObject>

class TurnManager : public QObject
{
  Q_OBJECT
public:
  virtual ~TurnManager();

  static TurnManager& getInstance()
  {
    static TurnManager instance;
    return instance;
  }

  static QSharedPointer<Player>& currentPlayer();
  static void switchPlayers(QSharedPointer<Player>& toWhichPlayer);

signals:
  void turnChanged(QSharedPointer<Player>& toWhichPlayer);

public slots:

private:
  explicit TurnManager(QObject* parent = 0);
  TurnManager(TurnManager const&);
  void operator=(TurnManager const&);

  static QSharedPointer<Player> _currentPlayer;
  void setCurrentPlayer(const QSharedPointer<Player>& currentPlayer);
};

#endif // TURNMANAGER_H
