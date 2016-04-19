/**
 * @file   MoveMapper.h
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains structure definitions and members of the MoveMapper class.
 */

#ifndef MOVEMAPPER_H
#define MOVEMAPPER_H

#include "Player.h"
#include "Board.h"

#include <QObject>

/**
 * Forward declaration of class Board.
 */
class Board;

/**
 * @brief The MoveMapper class is a single purpose class that detects end-game conditions.
 */
class MoveMapper : public QObject
{

  Q_OBJECT

public:

    /**
   * @brief ~MoveMapper is the default destructor for objects of type MoveMapper
   */
  virtual ~MoveMapper();

    /**
   * @brief getInstance returns an instance reference, as the constructor is private.  Singleton Pattern
   * @return an instance reference to the static object of class MoveMapper.
   */
  static MoveMapper& getInstance()
  {
    static MoveMapper instance;
    return instance;
  }

  /**
   * @brief doesPlayerHaveAvailableMoves calculates every possible move of every piece for a given player.
   * @param whichPlayer is the player that the calculation will be done for.
   * @param containerOfAvailableMoves [in,out] is the container of moves available, post analysis.
   * @param kingChecked [out] is a boolean that indicates whether the player referenced by whichPlayer's king is checked.
   * @param locationStart [out] is a boardCoordinateType that tells you the location of the piece whose valid moves are in containerOfAvailableMoves.
   * @param reverseIterate is a boolean that indicates whether containers will be accessed from the back or the front.
   * @param priorityForAttack is a boolean that indicates whether or not priority should be given to attack enemy pieces.
   * @return true if moves are available, false if no moves are available to the player.
   */
  bool doesPlayerHaveAvailableMoves(QSharedPointer<Player>& whichPlayer,
                                    boardCoordinatesType& containerOfAvailableMoves,
                                    bool* kingChecked,
                                    boardCoordinateType& locationStart,
                                    bool reverseIterate = false,
                                    bool priorityForAttack = false);

  /**
   * @brief associatedGameBoard is an accessor method to the Board pointer currently assiated with this instance of MoveMapper
   * @return a pointer to the associated Board.
   */
  Board* associatedGameBoard() const;

  /**
   * @brief associateGameBoard is a mutator method to manipulate the the associate game Board pointer.
   * @param associatedGameBoard is the new pointer to store as the associated game Board.
   */
  void associateGameBoard(Board* associatedGameBoard);

private:

  /**
   * @brief MoveMapper is the private default constructor for objects of MoveMapper.
   * @param parent is the QObject that will eventually destroy the pointer to MoveMapper (if not null).
   */
  explicit MoveMapper(QObject* parent = 0);

  /**
   * @brief MoveMapper declared as private, this copy- by-const-reference constructor is now no longer accessible.
   */
  MoveMapper(MoveMapper const&);

  /**
   * @brief operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
   */
  void operator=(MoveMapper const&);

  /**
   * @brief _theGameBoard is the private member where a pointer to the game Board is stored.
   */
  Board* _theGameBoard;

signals:

public slots:
};

#endif // MOVEMAPPER_H
