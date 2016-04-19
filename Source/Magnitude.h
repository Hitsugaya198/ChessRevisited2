/**
 * \file   Magnitude.h
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains the inner structure definition for the various movement magnitude rule aspects of Chess.
 *
 * The movement rules are handled per piece:
 * * Pawn   - Min 1, Max 2
 * * Castle - Min 1, Max 8
 * * Knight - Min 3, Max 3
 * * Bishop - Min 1, Max 8
 * * King   - Min 1, Max 1
 * * Queen  - Min 1, Max 8
 */

#ifndef MAGNITUDES
#define MAGNITUDES

/**
 * The Magnitude struct houses an enumeration that defines finite movement magnitudes to create movements with.
 */
struct Magnitude {

  /**
  * The eMagnitudes enum houses the movement magnitudes.
  */
  enum eMagnitudes {
    eOne   = 1, /**< enum value eOne   = 1 */
    eTwo   = 2, /**< enum value eTwo   = 2 */
    eThree = 3, /**< enum value eThree = 3 */
    eFour  = 4, /**< enum value eFour  = 4 */
    eFive  = 5, /**< enum value eFive  = 5 */
    eSix   = 6, /**< enum value eSix   = 6 */
    eSeven = 7, /**< enum value eSeven = 7 */
    eEight = 8  /**< enum value eEight = 8 */
             // No piece can move further than 8.
  };
};


#endif // MAGNITUDES

