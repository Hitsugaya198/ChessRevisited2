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

  static Pieces& getInstance()
  {
    static Pieces instance;
    return instance;
  }

  QStringList identityNames() const
  {
    return _identityNames;
  }

private:

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

  Pieces(Pieces const&);
  void operator=(Pieces const&);

  QStringList _identityNames;
};

#endif // PIECES_H



