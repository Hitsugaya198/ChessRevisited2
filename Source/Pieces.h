///
/// \file   Piece.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains structure definitions and members of the Pieces struct.
///

#ifndef PIECES_H
#define PIECES_H

#include <QStringList>

///
/// The Pieces struct defines the identity aspects of a Chess Piece.
///
struct Pieces {
public:
  struct Identities {
    enum eIdentities {
      eNone = 0,
      ePawn,
      eCastle,
      eKnight,
      eBishop,
      eQueen,
      eKing,
      eInit = eNone,
      eMax = eKing
    };
  };

  ///
  /// getInstance returns an instance reference, as the constructor is private.  Singleton Pattern.
  ///
  /// \return
  ///
  static Pieces& getInstance()
  {
    static Pieces instance;
    return instance;
  }

  ///
  /// identityNames is an accessor function to a QStringList that gives a string representation of the identity names in English.
  ///
  /// \return a QStringList that gives a string representation of the identity names in English.
  ///
  QStringList identityNames() const
  {
    return _identityNames;
  }

private:

  ///
  /// Pieces is the default constructor for struct Pieces and is used to initialize the _identityNames member.
  ///
  Pieces()
  {
    _identityNames = QStringList();
    _identityNames.append(QString("None"));
    _identityNames.append(QString("Pawn"));
    _identityNames.append(QString("Castle"));
    _identityNames.append(QString("Knight"));
    _identityNames.append(QString("Bishop"));
    _identityNames.append(QString("Queen"));
    _identityNames.append(QString("King"));

  }

  ///
  /// Pieces declared as private, this copy- by-const-reference constructor is now no longer accessible.
  ///
  Pieces(Pieces const&);

  ///
  /// operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
  ///
  void operator=(Pieces const&);

  ///
  /// _identityNames is the private member that stores the English names of the Piece Identities.
  ///
  QStringList _identityNames;
};

#endif // PIECES_H



