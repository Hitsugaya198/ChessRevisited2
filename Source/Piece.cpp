///
/// \file   Piece.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the inner management features of a Chess Piece.
///
/// In this cpp file is housed the functionality to construct and manage a Chess Piece.
///

#include "Piece.h"
#include <QDebug>

Piece::Piece(QObject* parent) : QObject(parent)
{
  setIdentity(Pieces::Identities::eNone);
  setColor(PieceColors::eNone);
  generatePixmap();
}

Piece::Piece(Pieces::Identities::eIdentities identity, PieceColors::ePieceColors pieceColor, QObject* parent) : QObject(parent)
{
  setIdentity(identity);
  setColor(pieceColor);
  generatePixmap();
}

Piece::~Piece()
{

}

Pieces::Identities::eIdentities Piece::identity() const
{
  return _identity;
}

void Piece::setIdentity(const Pieces::Identities::eIdentities& identity)
{
  _identity = identity;
}

PieceColors::ePieceColors Piece::color() const
{
  return _color;
}

void Piece::setColor(const PieceColors::ePieceColors& color)
{
  _color = color;
}

PieceColors::ePieceColors Piece::enemyColor() const
{
  return PieceColors::flipColor(color());
}

PieceColors::ePieceColors Piece::enemyColor(PieceColors::ePieceColors colorInput)
{
  return PieceColors::flipColor(colorInput);
}

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

const QPixmap& Piece::getPixmap() const
{
  return _pixmap;
}

void Piece::assignCell(Cell* cell)
{
  _myCell = (cell);
}

void Piece::clearAssignedCell()
{
  _myCell = 0;
}
