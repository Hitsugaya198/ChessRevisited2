#ifndef PIECES_H
#define PIECES_H

#include <QStringList>

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

  struct PieceColors {
    enum ePieceColors {
      eNone = 0,
      eBlack,
      eWhite,
      eInit = eNone,
      eMax = eWhite
    };
  };

  static Pieces& getInstance()
  {
    static Pieces instance;
    return instance;
  }


  QStringList colorNames() const
  {
    return _colorNames;
  }

  QStringList identityNames() const
  {
    return _identityNames;
  }

  static inline PieceColors::ePieceColors flipColor(PieceColors::ePieceColors color)
  {
    switch(color)
    {
      case PieceColors::eBlack : return PieceColors::eWhite;
      case PieceColors::eWhite : return PieceColors::eBlack;
      default                  : return PieceColors::eNone;
    }
  }

private:

  Pieces()
  {
    _colorNames = QStringList();
    _colorNames.append(QString("None"));
    _colorNames.append(QString("Black"));
    _colorNames.append(QString("White"));

    _identityNames = QStringList();
    _identityNames.append(QString("None"));
    _identityNames.append(QString("Pawn"));
    _identityNames.append(QString("Castle"));
    _identityNames.append(QString("Knight"));
    _identityNames.append(QString("Bishop"));
    _identityNames.append(QString("Queen"));
    _identityNames.append(QString("King"));

  }

  Pieces(Pieces const&);
  void operator=(Pieces const&);

  QStringList _colorNames;
  QStringList _identityNames;
};

#endif // PIECES_H



