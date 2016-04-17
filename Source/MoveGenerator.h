#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Player.h"
#include "Board.h"

#include <QObject>

class MoveGenerator : public QObject
{
  Q_OBJECT
public:
  explicit MoveGenerator(QObject* parent = 0);
  virtual ~MoveGenerator();

  QSharedPointer<Player> aiPlayer() const;
  void setAiPlayer(const QSharedPointer<Player>& aiPlayer);

  Board* associatedGameBoard() const;
  void associateGameBoard(Board* associatedGameBoard);

signals:
  void endGame(bool checkMate);

public slots:
  void handleTurnChange(QSharedPointer<Player>& itIsNowThisPlayersTurn);
  void handleMoveCompletionRequired();

private:
  QSharedPointer<Player> _aiPlayer;
  Board* _theGameBoard;
  boardCoordinateType _locationStart;
  boardCoordinateType _locationEnd;
  boardCoordinatesType _containerForMoving;
  static int _moveStrategy;
  static bool _moveFromFrontOfContainer;

  void moveTheFirstPieceThatCanMove(bool priorityForAttack = false);
  void moveWithPriorityForAttack();
  void moveAnyPawnThatCanMove();
};

#endif // MOVEGENERATOR_H
