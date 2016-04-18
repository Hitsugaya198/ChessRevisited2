#ifndef MOVEMAPPER_H
#define MOVEMAPPER_H

#include "Player.h"
#include "Board.h"

#include <QObject>

class Board;

class MoveMapper : public QObject
{
  Q_OBJECT
public:
  virtual ~MoveMapper();

  static MoveMapper& getInstance()
  {
    static MoveMapper instance;
    return instance;
  }

  bool doesPlayerHaveAvailableMoves(QSharedPointer<Player>& whichPlayer,
                                    boardCoordinatesType& containerOfAvailableMoves,
                                    bool* kingChecked,
                                    boardCoordinateType& locationStart,
                                    bool reverseIterate = false,
                                    bool priorityForAttack = false);

  Board* associatedGameBoard() const;
  void associateGameBoard(Board* associatedGameBoard);

private:
  explicit MoveMapper(QObject* parent = 0);
  MoveMapper(MoveMapper const&);
  void operator=(MoveMapper const&);

  Board* _theGameBoard;

signals:

public slots:
};

#endif // MOVEMAPPER_H
