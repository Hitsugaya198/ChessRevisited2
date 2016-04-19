/**
 * \file   Piece.h
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains structure definitions and members of the Piece class.
 */

#ifndef PIECE_H
#define PIECE_H

#include "Pieces.h"
#include "Cell.h"

#include <QObject>
#include <QPixmap>

/**
 * The Piece class structurally and functionally represents a Chess Piece.
 */
class Piece : public QObject
{

  Q_OBJECT

public:

   /**
   * Piece::Piece is the default constructor that can only create "blanks", required for an empty Board Cell.
   * \param parent is the QObject that will eventually destroy the Piece. If it is not null.
   */
  explicit Piece(QObject* parent = 0);

    /**
   * Piece::Piece is the default constructor that can create any piece, including "blanks".
   * \param identity is the identity of the Piece to be created.
   * \param pieceColor is the color of the Piece to be created.
   * \param parent is the QObject that will eventually destroy the Piece. If it is not null.
   */
  explicit Piece(Pieces::Identities::eIdentities identity, PieceColors::ePieceColors pieceColor, QObject* parent = 0);

    /**
   * Piece::~Piece is the default destructor for class Piece.
   */
  virtual ~Piece();

    /**
   * Piece::identity is an accessor method to query the identity of a Piece
   * \return the identity type of a piece defined in \file Pieces.h
   */
  Pieces::Identities::eIdentities identity() const;

  /**
   * Piece::setIdentity is a mutator method to manipulate the identity of a Piece
   * \param identity is the type of a piece defined in \file Pieces.h to set as the new identity of this Piece
   */
  void setIdentity(const Pieces::Identities::eIdentities& identity);

  /**
   * Piece::color is an accessor method to query the colour of a Piece
   * \return the color type of a piece defined in \file Colors.h
   */
  PieceColors::ePieceColors color() const;

  /**
   * Piece::setColor is a mutator method to manipulate the colour of a Piece
   * \param color is the colour type of a piece defined in \file Color.h to set as the new colour of this Piece
   */
  void setColor(const PieceColors::ePieceColors& color);

  /**
   * Piece::enemyColor is a conveniece function that flips this Piece's colour to determine that of his enemy.
   * \return the colour of the enemies of this Piece
   */
  PieceColors::ePieceColors enemyColor() const;

  /**
   * Piece::enemyColor is a static conveniece function that flips any colour to determine that of its enemy, when there is no Piece instance.
   * \param colorInput [in] a specific value to determine the enemy of.
   * \return the colour of the enemies of Pieces of colorInput colour.
   */
  static PieceColors::ePieceColors enemyColor(PieceColors::ePieceColors colorInput);

  /**
   * Piece::generatePixmap generates the internal pixmap image of a Piece that will ultimately be displayed as the icon of a Cell
   */
  void generatePixmap();

  /**
   * Piece::getPixmap is an accessor to the pixmap image embedded inside a Piece
   * \return the pixmap to be used for graphically representing a Piece
   */
  const QPixmap& getPixmap() const;

  /**
   * Piece::assignCell exists to make the cell aware of it's location, but isn't used for anything useful after it's assigned.
   * \param cell is the Cell that this piece knows it is on.
   */
  void assignCell(Cell* cell);

  /**
   * assignedCell is an accessor method that provides access to the cell that this piece associates with.
   * \return a Cell pointer to this Piece's assigned Piece.
   */
  Cell* assignedCell()
  {
    return _myCell;
  }

  /**
   * Piece::clearAssignedCell does what its name suggests.  This Piece dissociates itself from its Cell.
   */
  void clearAssignedCell();

signals:

public slots:

private:

  /**
   * _identity
   */
  Pieces::Identities::eIdentities _identity;

  /**
   * _color
   */
  PieceColors::ePieceColors _color;

  /**
   * _pixmap
   */
  QPixmap _pixmap;

  /**
   * _myCell
   */
  Cell* _myCell;
};

#endif // PIECE_H
