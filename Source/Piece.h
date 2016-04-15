#ifndef PIECE_H
#define PIECE_H

#include "Pieces.h"
#include "Cell.h"

#include <QObject>
#include <QPixmap>

class Piece : public QObject
{
  Q_OBJECT
public:
  typedef Pieces::Identities::eIdentities eIdentities;
  typedef Pieces::PieceColors::ePieceColors ePieceColors;

  explicit Piece(QObject* parent = 0);
  explicit Piece(eIdentities identity, ePieceColors pieceColor, QObject* parent = 0);
  virtual ~Piece();

  eIdentities identity() const;
  void setIdentity(const eIdentities& identity);

  ePieceColors color() const;
  void setColor(const ePieceColors& color);
  ePieceColors enemyColor() const;
  static ePieceColors enemyColor(Pieces::PieceColors::ePieceColors colorInput);

  void generatePixmap();
  const QPixmap& getPixmap() const;

  void assignCell(Cell* cell);
  Cell* assignedCell() { return _myCell; }
  void clearAssignedCell();

signals:

public slots:

private:
  eIdentities _identity;
  ePieceColors _color;
  QPixmap _pixmap;

  Cell* _myCell;
};

#endif // PIECE_H
