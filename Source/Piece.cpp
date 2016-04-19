/**
 * \file   Piece.cpp
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains the inner management features of a Chess Piece
 *
 * In this cpp file is housed the functionality to construct and manage a Chess Piece
 */

#include "Piece.h"
#include <QDebug>

///
/// Piece::Piece is the default constructor that can only create "blanks", required for an empty Board Cell.
/// \param parent is the QObject that will eventually destroy the Piece. If it is not null.
///
Piece::Piece(QObject* parent) : QObject(parent)
{
  setIdentity(Pieces::Identities::eNone);
  setColor(PieceColors::eNone);
  generatePixmap();
}

///
/// Piece::Piece is the default constructor that can create any piece, including "blanks".
/// \param identity is the identity of the Piece to be created.
/// \param pieceColor is the color of the Piece to be created.
/// \param parent is the QObject that will eventually destroy the Piece. If it is not null.
///
Piece::Piece(Pieces::Identities::eIdentities identity, PieceColors::ePieceColors pieceColor, QObject* parent) : QObject(parent)
{
  setIdentity(identity);
  setColor(pieceColor);
  generatePixmap();
}

///
/// Piece::~Piece is the default destructor for class Piece.
///
Piece::~Piece()
{

}

///
/// Piece::identity is an accessor method to query the identity of a Piece
/// \return the identity type of a piece defined in \file Pieces.h
///
Pieces::Identities::eIdentities Piece::identity() const
{
  return _identity;
}

///
/// Piece::setIdentity is a mutator method to manipulate the identity of a Piece
/// \param identity is the type of a piece defined in \file Pieces.h to set as the new identity of this Piece
///
void Piece::setIdentity(const Pieces::Identities::eIdentities& identity)
{
  _identity = identity;
}

///
/// Piece::color is an accessor method to query the colour of a Piece
/// \return the color type of a piece defined in \file Colors.h
///
PieceColors::ePieceColors Piece::color() const
{
  return _color;
}

///
/// Piece::setColor is a mutator method to manipulate the colour of a Piece
/// \param color is the colour type of a piece defined in \file Color.h to set as the new colour of this Piece
///
void Piece::setColor(const PieceColors::ePieceColors& color)
{
  _color = color;
}

///
/// Piece::enemyColor is a conveniece function that flips this Piece's colour to determine that of his enemy.
/// \return the colour of the enemies of this Piece
///
PieceColors::ePieceColors Piece::enemyColor() const
{
  return PieceColors::flipColor(color());
}

///
/// Piece::enemyColor is a static conveniece function that flips any colour to determine that of its enemy, when there is no Piece instance.
/// \param colorInput [in] a specific value to determine the enemy of.
/// \return the colour of the enemies of Pieces of colorInput colour.
///
PieceColors::ePieceColors Piece::enemyColor(PieceColors::ePieceColors colorInput)
{
  return PieceColors::flipColor(colorInput);
}

///
/// Piece::generatePixmap generates the internal pixmap image of a Piece that will ultimately be displayed as the icon of a Cell
///
void Piece::generatePixmap()
{
  if (this->color() == PieceColors::eNone) {
    return;
  }
  if (this->identity() == Pieces::Identities::eNone) {
    return;
  }

  QString colorString = PieceColors::getInstance().colorNames().at(this->color());
  QString identityString = Pieces::getInstance().identityNames().at(this->identity());
  QString resPath = QString(":/Pieces/") + QString("Resources/") + colorString + QString("/") + identityString + QString(".png");

  QPixmap pm;
  pm.load(resPath, "PNG");

  _pixmap = QPixmap(pm).scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

///
/// Piece::getPixmap is an accessor to the pixmap image embedded inside a Piece
/// \return the pixmap to be used for graphically representing a Piece
///
const QPixmap& Piece::getPixmap() const
{
  return _pixmap;
}

///
/// Piece::assignCell exists to make the cell aware of it's location, but isn't used for anything useful after it's assigned.
/// \param cell is the Cell that this piece knows it is on.
///
void Piece::assignCell(Cell* cell)
{
  _myCell = (cell);
}

///
/// Piece::clearAssignedCell does what its name suggests.  This Piece dissociates itself from its Cell.
///
void Piece::clearAssignedCell()
{
  _myCell = 0;
}
