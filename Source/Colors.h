///
/// \file   Colors.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file contains the inner structure definitions for the various color aspects of Chess.
///
/// At the time of writing, only two colour types were involved.  That of the Cell, and that of the Piece.
///

#ifndef COLORS_H
#define COLORS_H

#include <QString>

///
/// The CellColors struct contains properties and functions related to the colouring of Board Cells.
///
struct CellColors {

public:

  ///
  /// The eCellColors enum outlines the values of colours that a Board Cell can be.
  ///
  enum eCellColors {
    eNone,             ///< enum value eNone eludes to an unitialized state.
    eBrown,            ///< enum value eBrown refers to the colour brown.
    eBeige,            ///< enum value eBeige refers to the colour beige.
    eInit  = eBrown,   ///< enum value eInit is useful to always initialize elements to a valid state.
    eMax   = eBeige    ///< enum value eMax implies it is the last colour in the range.
  };

  ///
  /// getInstance returns an instance reference, as the constructor is private.  Singleton Pattern.
  ///
  /// \return an instance reference to the static object of struct CellColors.
  ///
  static CellColors& getInstance()
  {
    static CellColors instance;
    return instance;
  }

  ///
  /// getBrown accessor method to obtain the color code for brown.
  ///
  /// \return the html color code as a string.
  ///
  QString getBrown()
  {
    return _htmlColorCodeForBrown;
  }

  ///
  /// getBeige accessor method to obtain the color code for beige.
  ///
  /// \return the html color code as a string.
  ///
  QString getBeige()
  {
    return _htmlColorCodeForBeige;
  }

  ///
  /// getHiBrown accessor method to obtain the color code for a lighter shade of brown.
  ///
  /// \return the html color code as a string.
  ///
  QString getHiBrown()
  {
    return _htmlColorCodeForHighlightBrown;
  }

  ///
  /// getHiBeige accessor method to obtain the color code for a lighter shade of beige.
  ///
  /// \return the html color code as a string.
  ///
  QString getHiBeige()
  {
    return _htmlColorCodeForHighlightBeige;
  }

private:

  ///
  /// CellColors is the private default constructor for objects of CellColors.
  ///
  CellColors()
  {
    _htmlColorCodeForBrown = QString("#663300");
    _htmlColorCodeForBeige = QString("#EDC25E");
    _htmlColorCodeForHighlightBeige = QString("#F5DCA3");
    _htmlColorCodeForHighlightBrown = QString("#994D00");
  }

  ///
  /// CellColors declared as private, this copy- by-const-reference constructor is now no longer accessible.
  ///
  CellColors(CellColors const&);

  ///
  /// operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
  ///
  void operator=(CellColors const&);

  ///
  /// _htmlColorCodeForBeige is the private member that stores the html color code for beige.
  ///
  QString _htmlColorCodeForBeige;

  ///
  /// _htmlColorCodeForBrown is the private member that stores the html color code for brown.
  ///
  QString _htmlColorCodeForBrown;

  ///
  /// _htmlColorCodeForHighlightBeige is the private member that stores the html color code for a lighter shade of beige.
  ///
  QString _htmlColorCodeForHighlightBeige;

  ///
  /// _htmlColorCodeForHighlightBrown is the private member that stores the html color code for a lighter shade of brown.
  ///
  QString _htmlColorCodeForHighlightBrown;
};

///
/// The PieceColors struct contains properties and functions related to the colouring of Chess Pieces.
///
struct PieceColors {

  ///
  /// The ePieceColors enum outlines the values of colours that a Chess Piece can be.
  ///
  enum ePieceColors {
    eNone  = 0,      ///< enum value eNone eludes to an uninitialized state.
    eBlack,          ///< enum value eBlack refers to the colour Black.
    eWhite,          ///< enum value eWhite refers to the colour White.
    eInit  = eNone,  ///< enum value eInit is useful to always initialize elements to a valid state.
    eMax   = eWhite  ///< enum value eMax implies it is the last colour in the range.
  };

  ///
  /// getInstance returns an instance reference, as the constructor is private.  Singleton Pattern.
  ///
  /// \return a static instance of struct PieceColors.
  ///
  static PieceColors& getInstance()
  {
    static PieceColors instance;
    return instance;
  }

  ///
  /// flipColor provides a quick binary outcome to the question "What's the opposite of?".
  ///
  /// \param color is the colour that is to be flipped.
  /// \return the opposite to color.
  ///
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

  ///
  /// colorNames a QStringList that gives a string representation of the colour names in English.
  ///
  /// \return a copy of the QStringList.
  ///
  QStringList colorNames() const
  {
    return _colorNames;
  }

private:

  ///
  /// PieceColors is the private default constructor of struct PieceColors.
  ///
  /// It instantiates and populates the _colorNames list.
  ///
  PieceColors()
  {
    _colorNames = QStringList();
    _colorNames.append(QString("None"));
    _colorNames.append(QString("Black"));
    _colorNames.append(QString("White"));
  }

  ///
  /// PieceColors declared as private, this copy- by-const-reference constructor is now no longer accessible.
  ///
  PieceColors(PieceColors const&);

  ///
  /// operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
  ///
  void operator=(PieceColors const&);

  ///
  /// _colorNames is the private member that stores the English names of the Piece Colors.
  ///
  QStringList _colorNames;

};

#endif // COLORS_H
