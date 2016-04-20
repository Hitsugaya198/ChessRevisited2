///
/// \file   Cell.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the inner management features of a Cell on a Chess Board.
///
/// In this cpp file is housed all the functions and attributes needed to construct and manage a
/// a Chess Board Cell.
///
/// Most notably, a Cell is actually a styleable QPushButton, that is also defined to be checkable.
/// It embeds a Chess Piece into itself, and sets an image of that Piece as its icon.
///

#include "Cell.h"

#include "StyleSheetProcessor.h"
#include "Colors.h"
#include "Piece.h"
#include "TurnManager.h"

#include <QGridLayout>
#include <QDebug>

int Cell::_checkedCounter = 0;

Cell::Cell(QWidget* parent):
  QPushButton(parent),
  _myPiece(new Piece(parent))
{
  // We want to know when a selection takes place.  This is indicated by a toggled signal
  connect(this, SIGNAL(toggled(bool)), this, SLOT(handleCellToggled(bool)));
}

Cell::~Cell()
{

}

rowType Cell::row() const
{
  return _row;
}

void Cell::setRow(rowType row)
{
  _row = row;
}

columnType Cell::column() const
{
  return _column;
}

void Cell::setColumn(columnType column)
{
  _column = column;
}

void Cell::toggleCellHighlights(bool highLight)
{
  setColor(highLight);
}

void Cell::resetCheckedCounter()
{
  _checkedCounter = 0;
}

bool Cell::checkAccessContinue()
{
  // Check that the right user is attempting to move
  // Pieces must be of the enemy color or empty
  if (assignedPiece()->color() != PieceColors::flipColor(TurnManager::currentPlayer()->associatedColor())) {
    if (assignedPiece()->color() != PieceColors::eNone) {
      setChecked(false);
      toggleCellHighlights(false);
      return false;
    }
  }

  return true;
}

bool Cell::checkAccessInit()
{
  // Check that the right user is attempting to move.
  // Pieces must be of the associated color.
  if (assignedPiece()->color() != TurnManager::currentPlayer()->associatedColor()) {
    setChecked(false);
    toggleCellHighlights(false);
    return false;
  }

  return true;
}

void Cell::handleCellToggled(bool checked)
{
  // Starting a new move
  if (_checkedCounter == 0 && checked) {
    ++_checkedCounter;
    if (!checkAccessInit()) {
      --_checkedCounter;
      return;
    }
    toggleCellHighlights(true);
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
  toggleCellHighlights(false);
  emit nothingToDo();
}

void Cell::setCoordinate(rowType row, columnType column)
{
  setRow(row);
  setColumn(column);

  setColor(false);
}

void Cell::setColor(bool highLight)
{
  // Remove current stylesheet effects.
  style()->unpolish(this);

  QString s = styleSheet();                       // This line retrieves your current StyleSheet for THIS widget.
  splitStyleMapType map;     // Creates a map to store your stylesheet elements
  map = StyleSheetProcessor::splitStyleSheet(s);  // Assigns your processed stylesheet to the local map

  QString brownColor;        //  Boringly enough, this Chess Board will have
  QString beigeColor;        //  brown and beige cells only.

  if (highLight) { // If highlighted, the colors need to be of the Hi variety.
    brownColor = CellColors::getInstance().getHiBrown();
    beigeColor = CellColors::getInstance().getHiBeige();
  }
  else { // If not highlighted, use the normal boring shades.
    brownColor = CellColors::getInstance().getBrown();
    beigeColor = CellColors::getInstance().getBeige();
  }

  // The row/column coordinate's divisibility by two determines the color of the Cell.
  // In summary, all even row even column cells will be brown with beige borders, and all
  // even row uneven column cells will be beige with brown borders.
  if (_row % 2 == 0) { // rows 2, 4, 6, and 8
    if (_column % 2 == 0) { // columns 2, 4, 6, and 8
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBackGroundColor, brownColor);    // Replaces the stylesheet element for background color with "brownColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBorderColor, beigeColor);        // Replaces the stylesheet element for border color with "beigeColor"
    }
    else { // columns  1, 3, 5, and 7
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBackGroundColor, beigeColor);    // Replaces the stylesheet element for background color with "beigeColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBorderColor, brownColor);        // Replaces the stylesheet element for border color with "brownColor"
    }
  }
  // And all uneven row uneven column cells will be brown with beige borders, and all
  // uneven row even column cells will be beige with brown borders.
  else if (_row % 2 != 0) { // rows 1, 3, 5, and 7
    if (_column % 2 != 0) {// columns  1, 3, 5, and 7
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBackGroundColor, brownColor);    // Replaces the stylesheet element for background color with "brownColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBorderColor, beigeColor);        // Replaces the stylesheet element for border color with "beigeColor"
    }
    else { // columns 2, 4, 6, and 8
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBackGroundColor, beigeColor);    // Replaces the stylesheet element for background color with "beigeColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, sBorderColor, brownColor);        // Replaces the stylesheet element for border color with "brownColor"
    }
  }

  // Solid boder style flattens the button, so it doesn't look like, well, a button.
  StyleSheetProcessor::addOrReplaceItemInMap(map, sBorderStyle, QString("solid"));       // Replaces the stylesheet element for border style with "solid"
  StyleSheetProcessor::addOrReplaceItemInMap(map, sBorderWidth, QString("1px"));         // Replaces the stylesheet element for border width with "1px"

  s = StyleSheetProcessor::mergeStyleSheet(map);                                                              // Merges the stylesheet back into a single string.
  setStyleSheet(s);        // Sets the stylesheet of THIS widget.
  style()->polish(this);   // Adds back stylesheet effects.
}

void Cell::assignPiece(QSharedPointer<Piece> piece)
{
  clearAssignedPiece(); // make sure there is nothing currently assigned.

  _myPiece = QSharedPointer<Piece>(piece); // Copy construct my inner Piece.

  // Force the size of the icon to be 40x40
  QSize mySize(40, 40);
  setIconSize(mySize);
  setIcon(_myPiece->getPixmap());
  _myPiece->assignCell(this);
}

QSharedPointer<Piece>& Cell::assignedPiece()
{
  return _myPiece;
}

boardCoordinateType Cell::position()
{
  return boardCoordinateType(row(), column());
}

void Cell::clearAssignedPiece()
{
  _myPiece.clear();
  _myPiece = QSharedPointer<Piece>(new Piece);
  setIcon(QPixmap());
}
