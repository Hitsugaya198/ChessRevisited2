#ifndef DIRECTIONS
#define DIRECTIONS

/**
* @brief The Direction struct
*/
struct Direction {

  /**
  * @brief The eDirectionRules enum
  */
  enum eDirectionRules {
    eMayMoveNorth,
    eMayMoveNorthEast,
    eMayMoveEast,
    eMayMoveSouthEast,
    eMayMoveSouth,
    eMayMoveSouthWest,
    eMayMoveWest,
    eMayMoveNorthWest,
    eInit = eMayMoveNorth,
    eMax = eMayMoveNorthWest
  };
};

#endif // DIRECTIONS

