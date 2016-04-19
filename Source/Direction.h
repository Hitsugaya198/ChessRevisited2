/**
 * \file   Direction.h
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains the inner structure definition for the various movement direction rule aspects of Chess.
 *
 * The movement rules are defined in line with the four major and four minor wind directions of a compass.
 * The mapping is fixed with row A colum 1 at the North Western corner, and row H column 8 at the South Eastern
 * corner of the Board.
 */

#ifndef DIRECTIONS
#define DIRECTIONS

/**
* The Direction struct houses an enumeration that defines finite movement rules to create movements with.
*/
struct Direction {

  /**
  * The eDirectionRules enum houses the movement direction rules.
  */
  enum eDirectionRules {
    eMayMoveNorth,           /**< enum value eMayMoveNorth represents moving from row H towards row A while remaining in the same column */
    eMayMoveNorthEast,       /**< enum value eMayMoveNorthEast represents moving from row H towards row A while moving column from 1 towards 8 */
    eMayMoveEast,            /**< enum value eMayMoveEast represents moving from column 1 towards column 8 while remaining in the same row */
    eMayMoveSouthEast,       /**< enum value eMayMoveSouthEast represents moving from row A towards row H while moving column from 1 towards 8 */
    eMayMoveSouth,           /**< enum value eMayMoveSouth represents moving from row A towards row H while remaining in the same column */
    eMayMoveSouthWest,       /**< enum value eMayMoveSouthWest represents moving from row A towards row H while moving column from 8 towards 1 */
    eMayMoveWest,            /**< enum value eMayMoveWest represents moving from column 8 towards column 1 while remaining in the same row */
    eMayMoveNorthWest,       /**< enum value eMayMoveNorthWest represents moving from row H towards row A while moving column from 8 towards 1 */
    eInit = eMayMoveNorth,   /**< enum value eInit is useful to always initialize elements to a valid state */
    eMax = eMayMoveNorthWest /**< enum value eMax implies it is the last direction rule in the range */
  };
};

#endif // DIRECTIONS

