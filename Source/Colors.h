/**
 * @file   Colors.h
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the inner structure definitions for the various color aspects of Chess.
 *
 * At the time of writing, only two colour types were involved.  That of the Cell, and that of the Piece.
 */

#ifndef COLORS_H
#define COLORS_H

#include <QString>

/**
 * @brief The CellColors struct contains properties and functions related to the colouring of Board Cells
 */
struct CellColors {

public:
  /**
  * @brief The eCellColors enum outlines the values of colours that a Board Cell can be.
  */
  enum eCellColors {
    eNone,             /**< enum value eNone eludes to an unitialized state */
    eBrown,            /**< enum value eBrown refers to the colour brown */
    eBeige,            /**< enum value eBeige refers to the colour beige */
    eInit  = eBrown,   /**< enum value eInit is useful to always initialize elements to a valid state */
    eMax   = eBeige    /**< enum value eMax implies it is the last colour in the range */
  };

  /**
   * @brief getInstance returns an instance reference, as the constructor is private.  Singleton Pattern
   * @return an instance reference to the static object of struct CellColors.
   */
  static CellColors& getInstance()
  {
    static CellColors instance;
    return instance;
  }

  /**
   * @brief getBrown accessor method to obtain the color code for brown.
   * @return
   */
  QString getBrown()
  {
    return _htmlColorCodeForBrown;
  }

  /**
   * @brief getBeige accessor method to obtain the color code for beige.
   * @return
   */
  QString getBeige()
  {
    return _htmlColorCodeForBeige;
  }

  /**
   * @brief getHiBrown accessor method to obtain the color code for a lighter shade of brown.
   * @return
   */
  QString getHiBrown()
  {
    return _htmlColorCodeForHighlightBrown;
  }

  /**
   * @brief getHiBeige accessor method to obtain the color code for a lighter shade of beige.
   * @return
   */
  QString getHiBeige()
  {
    return _htmlColorCodeForHighlightBeige;
  }

private:

  /**
   * @brief CellColors is the private default constructor for objects of CellColors
   */
  CellColors()
  {
    _htmlColorCodeForBrown = QString("#663300");
    _htmlColorCodeForBeige = QString("#EDC25E");
    _htmlColorCodeForHighlightBeige = QString("#F5DCA3");
    _htmlColorCodeForHighlightBrown = QString("#994D00");
  }

  /**
   * @brief CellColors declared as private, this copy- by-const-reference constructor is now no longer accessible.
   */
  CellColors(CellColors const&);

  /**
   * @brief operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
   */
  void operator=(CellColors const&);

  /**
   * @brief _htmlColorCodeForBeige is the private member that stores the html color code for beige
   */
  QString _htmlColorCodeForBeige;

  /**
   * @brief _htmlColorCodeForBrown is the private member that stores the html color code for brown
   */
  QString _htmlColorCodeForBrown;

  /**
   * @brief _htmlColorCodeForHighlightBeige is the private member that stores the html color code for a lighter shade of beige.
   */
  QString _htmlColorCodeForHighlightBeige;

  /**
   * @brief _htmlColorCodeForHighlightBrown is the private member that stores the html color code for a lighter shade of brown.
   */
  QString _htmlColorCodeForHighlightBrown;
};

/**
 * @brief The PieceColors struct contains properties and functions related to the colouring of Chess Pieces
 */
struct PieceColors {

  /**
  * @brief The ePieceColors enum outlines the values of colours that a Chess Piece can be.
  */
  enum ePieceColors {
    eNone  = 0,      /**< enum value eNone eludes to an uninitialized state */
    eBlack,          /**< enum value eBlack refers to the colour Black */
    eWhite,          /**< enum value eWhite refers to the colour White */
    eInit  = eNone,  /**< enum value eInit is useful to always initialize elements to a valid state */
    eMax   = eWhite  /**< enum value eMax implies it is the last colour in the range */
  };

  /**
   * @brief getInstance returns an instance reference, as the constructor is private.  Singleton Pattern
   * @return
   */
  static PieceColors& getInstance()
  {
    static PieceColors instance;
    return instance;
  }

  /**
   * @brief flipColor provides a quick binary outcome to the question "What's the opposite of?"
   * @param color is the colour that is to be flipped.
   * @return the opposite to color
   */
  static inline PieceColors::ePieceColors flipColor(PieceColors::ePieceColors color)
  {
    switch (color) {
    case PieceColors::eBlack :
      return PieceColors::eWhite;
    case PieceColors::eWhite :
      return PieceColors::eBlack;
    default                  :
      return PieceColors::eNone;
    }
  }

  /**
   * @brief colorNames a QStringList that gives a string representation of the colour names in English
   * @return a copy of the QStringList
   */
  QStringList colorNames() const
  {
    return _colorNames;
  }

private:

  /**
   * @brief PieceColors is the private default constructor of struct PieceColors
   * It instantiates and populates the _colorNames list.
   */
  PieceColors()
  {
    _colorNames = QStringList();
    _colorNames.append(QString("None"));
    _colorNames.append(QString("Black"));
    _colorNames.append(QString("White"));
  }

  /**
   * @brief PieceColors declared as private, this copy- by-const-reference constructor is now no longer accessible.
   */
  PieceColors(PieceColors const&);

  /**
   * @brief operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
   */
  void operator=(PieceColors const&);

  /**
   * @brief _colorNames is the private member that stores the English names of the Piece Colors
   */
  QStringList _colorNames;

};

#endif // COLORS_H
