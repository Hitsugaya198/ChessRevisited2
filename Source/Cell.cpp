#include "Cell.h"

#include "StyleSheetProcessor.h"
#include "Colors.h"
#include "Piece.h"
#include "TurnManager.h"

#include <QGridLayout>
#include <QDebug>
#include <QMutexLocker>

int Cell::_checkedCounter = 0;

///
/// \brief Cell::Cell
/// \param parent
///
Cell::Cell(QWidget* parent):
  QPushButton(parent),
  _myPiece(new Piece(parent))
{
  _me = this;

  // We want to know when a selection takes place.  This is indicated by a toggled signal
  connect(this, SIGNAL(toggled(bool)), this, SLOT(handleCellToggled(bool)));
}

///
/// \brief Cell::~Cell
///
Cell::~Cell()
{

}

///
/// \brief Cell::row
/// \return
///
int Cell::row() const
{
  return _row;
}

///
/// \brief Cell::setRow
/// \param row
///
void Cell::setRow(int row)
{
  _row = row;
}

///
/// \brief Cell::column
/// \return
///
int Cell::column() const
{
  return _column;
}

///
/// \brief Cell::setColumn
/// \param column
///
void Cell::setColumn(int column)
{
  _column = column;
}

///
/// \brief Cell::highLightCell
/// \param highLight
///
void Cell::highLightCell(bool highLight)
{
  setColor(highLight);
}

///
/// \brief Cell::resetCheckedCounter
///
void Cell::resetCheckedCounter()
{
  _checkedCounter = 0;
}

bool Cell::checkAccessContinue()
{
  // Check that the right user is attempting to move
  if (TurnManager::currentPlayer()->identity() == UserIdentity::eHuman) {
    // Pieces must be Black or empty
    if (assignedPiece()->color() != Pieces::PieceColors::eBlack) {
      if (assignedPiece()->color() != Pieces::PieceColors::eNone) {
        setChecked(false);
        highLightCell(false);
        return false;
      }
    }
  }

  if (TurnManager::currentPlayer()->identity() == UserIdentity::eComputer) {
    // Pieces must be White or empty
    if (assignedPiece()->color() != Pieces::PieceColors::eWhite) {
      if (assignedPiece()->color() != Pieces::PieceColors::eNone) {
        setChecked(false);
        highLightCell(false);
        return false;
      }
    }
  }
  return true;
}

bool Cell::checkAccessInit()
{
  // Check that the right user is attempting to move
  if (TurnManager::currentPlayer()->identity() == UserIdentity::eHuman) {
    // Pieces must be white
    if (assignedPiece()->color() != Pieces::PieceColors::eWhite) {
      setChecked(false);
      highLightCell(false);
      return false;
    }
  }

  if (TurnManager::currentPlayer()->identity() == UserIdentity::eComputer) {
    // Pieces must be Black
    if (assignedPiece()->color() != Pieces::PieceColors::eBlack) {
      setChecked(false);
      highLightCell(false);
      return false;
    }
  }
  return true;
}

///
/// \brief Cell::handleCellToggled
/// \param checked
///
void Cell::handleCellToggled(bool checked)
{
  // Starting a new move
  if (_checkedCounter == 0 && checked) {
    ++_checkedCounter;
    if (!checkAccessInit()) {
      --_checkedCounter;
      return;
    }
    highLightCell(true);
    emit startingANewMove(position());
    return;
  }
  // Continuing previous move
  else if (_checkedCounter == 1 && checked) {
    if (!checkAccessContinue()) {
      ++_checkedCounter;
      return;
    }
    --_checkedCounter;
    emit completingMove(position());
    return;
  }
  // Unchecking a cell (changed your mind?)
  else if (_checkedCounter == 1 && !checked) {
    --_checkedCounter;
    if (!checkAccessInit()) {
      ++_checkedCounter;
      return;
    }
  }
  else if (_checkedCounter > 1 || _checkedCounter < 0) {
    _checkedCounter = 0;
  }
  highLightCell(false);
  emit nothingToDo();
}

///
/// \brief Cell::setCoordinate
/// \param row
/// \param column
///
void Cell::setCoordinate(int row, int column)
{
  QMutexLocker locker(&_setCoordinateMutex);
  setRow(row);
  setColumn(column);

  setColor(false);
}

///
/// \brief Cell::setColor
/// \param highLight
///
void Cell::setColor(bool highLight)
{
  QMutexLocker locker(&_setColorMutex);

  style()->unpolish(this);

  QString s = styleSheet();                       // This line retrieves your current StyleSheet for THIS widget.
  StyleSheetProcessor::splitStyleMapType map;     // Creates a map to store your stylesheet elements
  map = StyleSheetProcessor::splitStyleSheet(s);  // Assigns your processed stylesheet to the local map

  QString brownColor;
  QString beigeColor;
  QString brownBorderColor;
  QString beigeBorderColor;

  if (highLight) {
    brownColor = Colors::getInstance().getHiBrown();
    beigeColor = Colors::getInstance().getHiBeige();
    brownBorderColor = Colors::getInstance().getHiBrown();
    beigeBorderColor = Colors::getInstance().getHiBeige();
  }
  else {
    brownColor = Colors::getInstance().getBrown();
    beigeColor = Colors::getInstance().getBeige();
    brownBorderColor = Colors::getInstance().getBrown();
    beigeBorderColor = Colors::getInstance().getBeige();
  }

  // And then use that "id" to determine the color.
  if (_row % 2 == 0) {
    if (_column % 2 == 0) {
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, brownColor);    // Replaces the stylesheet element for background color with "#663300"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, beigeBorderColor);        // Replaces the stylesheet element for background color with "#EDC25E"
    }
    else {
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, beigeColor);    // Replaces the stylesheet element for background color with "#EDC25E"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, brownBorderColor);        // Replaces the stylesheet element for border color with "#663300"
    }
  }

  if (_row % 2 != 0) {
    if (_column % 2 != 0) {
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, brownColor);    // Replaces the stylesheet element for background color with "#663300"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, beigeBorderColor);        // Replaces the stylesheet element for background color with "#EDC25E"
    }
    else {
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, beigeColor);    // Replaces the stylesheet element for background color with "#EDC25E"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, brownBorderColor);        // Replaces the stylesheet element for border color with "#663300"
    }
  }

  // Removed the border styling, as it removes the visual effect of the cells being checked
  StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderStyle, QString("solid"));       // Replaces the stylesheet element for border style with "solid"
  StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderWidth, QString("1px"));
  //  StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderStyle, QString("outset"));      // Replaces the stylesheet element for border style with "outset"

  s = StyleSheetProcessor::mergeStyleSheet(map);                                                              // Merges the stylesheet back into a single string.
  setStyleSheet(s);        // Sets the stylesheet of THIS widget.
  style()->polish(this);   // Adds back stylesheet effects.
}

///
/// \brief Cell::assignPiece
/// \param piece
///
void Cell::assignPiece(QSharedPointer<Piece> piece)
{
  QMutexLocker locker(&_assignPieceMutex);
  clearAssignedPiece();

  _myPiece = QSharedPointer<Piece>(piece);
  QSize mySize(40, 40);
  setIconSize(mySize);
  setIcon(_myPiece->getPixmap());
  _myPiece->assignCell(this);
}

///
/// \brief Cell::assignedPiece
/// \return
///
QSharedPointer<Piece>& Cell::assignedPiece()
{

  return _myPiece;

}

///
/// \brief Cell::position
/// \return
///
Cell::boardCoordinateType Cell::position()
{
  return boardCoordinateType(row(), column());
}

///
/// \brief Cell::clearAssignedPiece
///
void Cell::clearAssignedPiece()
{
  QMutexLocker locker(&_clearAssignPieceMutex);
  _myPiece.clear();
  _myPiece = QSharedPointer<Piece>(new Piece);
  setIcon(QPixmap());
}
