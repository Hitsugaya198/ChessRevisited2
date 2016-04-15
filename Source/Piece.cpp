#include "Piece.h"
#include <QDebug>

///
/// \brief Piece::Piece
/// \param parent
///
Piece::Piece(QObject* parent) : QObject(parent)
{
  setIdentity(Pieces::Identities::eNone);
  setColor(Pieces::PieceColors::eNone);
  generatePixmap();
}

///
/// \brief Piece::Piece
/// \param identity
/// \param pieceColor
/// \param parent
///
Piece::Piece(Piece::eIdentities identity, Piece::ePieceColors pieceColor, QObject* parent) : QObject(parent)
{
  setIdentity(identity);
  setColor(pieceColor);
  generatePixmap();
}

///
/// \brief Piece::~Piece
///
Piece::~Piece()
{

}

///
/// \brief Piece::identity
/// \return
///
Pieces::Identities::eIdentities Piece::identity() const
{
  return _identity;
}

///
/// \brief Piece::setIdentity
/// \param identity
///
void Piece::setIdentity(const eIdentities& identity)
{
  _identity = identity;
}

///
/// \brief Piece::color
/// \return
///
Pieces::PieceColors::ePieceColors Piece::color() const
{
  return _color;
}

///
/// \brief Piece::setColor
/// \param color
///
void Piece::setColor(const ePieceColors& color)
{
  _color = color;
}

Piece::ePieceColors Piece::enemyColor() const
{
  if(color() == Pieces::PieceColors::eWhite)
  {
    return Pieces::PieceColors::eBlack;
  }
  else
  {
    return Pieces::PieceColors::eWhite;
  }
}

Piece::ePieceColors Piece::enemyColor(Pieces::PieceColors::ePieceColors colorInput)
{
  if(colorInput == Pieces::PieceColors::eWhite)
  {
    return Pieces::PieceColors::eBlack;
  }
  else
  {
    return Pieces::PieceColors::eWhite;
  }
}

///
/// \brief Piece::generatePixmap
///
void Piece::generatePixmap()
{
  if(this->color() == Pieces::PieceColors::eNone)
  {
    return;
  }
  if(this->identity() == Pieces::Identities::eNone)
  {
    return;
  }

  QString colorString = Pieces::getInstance().colorNames().at(this->color());
  QString identityString = Pieces::getInstance().identityNames().at(this->identity());
  QString resPath = QString(":/Pieces/") + QString("Resources/") + colorString + QString("/") + identityString + QString(".png");

  QPixmap pm;
  pm.load(resPath, "PNG");

//   qDebug() << resPath;

  _pixmap = QPixmap(pm).scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

///
/// \brief Piece::getPixmap
/// \return
///
const QPixmap& Piece::getPixmap() const
{
  return _pixmap;
}

///
/// \brief Piece::assignCell
/// \param cell
///
void Piece::assignCell(Cell* cell)
{
  _myCell = (cell);
}

///
/// \brief Piece::clearAssignedCell
///
void Piece::clearAssignedCell()
{
  _myCell = 0;
}
