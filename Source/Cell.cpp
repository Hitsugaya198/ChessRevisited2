/**
 * @file   Cell.cpp
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the inner management features of a Cell on a Chess Board
 *
 * In this cpp file is housed all the functions and attributes needed to construct and manage a
 * a Chess Board Cell.
 *
 * Most notably, a Cell is actually a styleable QPushButton, that is also defined to be checkable.
 * It embeds a Chess Piece into itself, and sets an image of that Piece as its icon.
 */

#include "Cell.h"

#include "StyleSheetProcessor.h"
#include "Colors.h"
#include "Piece.h"
#include "TurnManager.h"

#include <QGridLayout>
#include <QDebug>

///
/// \brief Cell::_checkedCounter is a static member to help Cell objects determine if other Cells were clicked before them
///
int Cell::_checkedCounter = 0;

///
/// \brief Cell::Cell() The default constructor for a Cell object.
/// \param parent [in] represents the object that will destroy instances of Cell, if not null. The Cells should have a Board as their parent.
///
Cell::Cell(QWidget* parent):
  QPushButton(parent),
  _myPiece(new Piece(parent))
{
  // We want to know when a selection takes place.  This is indicated by a toggled signal
  connect(this, SIGNAL(toggled(bool)), this, SLOT(handleCellToggled(bool)));
}

///
/// \brief Cell::~Cell() The default destructor that destroys Cell objects.
///
Cell::~Cell()
{

}

///
/// \brief Cell::row() is the accessor function that provides the row in which this Cell is located.
/// \return the row number where this Cell resides.
///
rowType Cell::row() const
{
  return _row;
}

///
/// \brief Cell::setRow() is the mutator function that manipulates the row in which this Cell is located.
/// \param row [in] the row number to set for this Cell.
///
void Cell::setRow(rowType row)
{
  _row = row;
}

///
/// \brief Cell::column() is the accessor function that provides the column in which this Cell is located.
/// \return the column number where this Cell resides.
///
columnType Cell::column() const
{
  return _column;
}

///
/// \brief Cell::setColumn() is the mutator function that manipulates the column in which this Cell is located.
/// \param column [in] the column number to set for this Cell.
///
void Cell::setColumn(columnType column)
{
  _column = column;
}

///
/// \brief Cell::highLightCell() changes the color of the Cell between highlighted and regular.
/// \param highLight [in] indicates whether to highlight the Cell or not.
/// This function can also be used to color cells by calling @code cell->highLightCell(false); @endcode
/// The reason for this is that highlights are merely a lighter shade of the normal color of the Cell,
/// and not highlighting it is the same as changing it's color to a darker shade of the highlighted color.
/// The actual color of the Cell is determined by its position on the board.
///
void Cell::highLightCell(bool highLight)
{
  setColor(highLight);
}

///
/// \brief Cell::resetCheckedCounter() is used to reset the checked counter of all Cell objects back to zero (static member).
///
void Cell::resetCheckedCounter()
{
  _checkedCounter = 0;
}

///
/// \brief Cell::checkAccessContinue() checks the location access of move destination Cell objects
/// \return true if the moving piece has access to the destination Cell, false if not.
///
bool Cell::checkAccessContinue()
{
  // Check that the right user is attempting to move
  // Pieces must be of the enemy color or empty
  if (assignedPiece()->color() != PieceColors::flipColor(TurnManager::currentPlayer()->associatedColor())) {
    if (assignedPiece()->color() != PieceColors::eNone) {
      setChecked(false);
      highLightCell(false);
      return false;
    }
  }

  return true;
}

///
/// \brief Cell::checkAccessInit() checks the location access of move origin Cell objects
/// \return true if the player has access to the origin Cell, false if not.
///
bool Cell::checkAccessInit()
{
  // Check that the right user is attempting to move.
  // Pieces must be of the associated color.
  if (assignedPiece()->color() != TurnManager::currentPlayer()->associatedColor()) {
    setChecked(false);
    highLightCell(false);
    return false;
  }

  return true;
}

///
/// \brief Cell::handleCellToggled() executes when a Cell is toggled (checked or unchecked)
/// \param checked [in] indicates whether the cell is being selected or unselected.
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
/// \brief Cell::setCoordinate() is a combined mutator function for allocating a Cell to a Board coordinate.
/// \param row [in] is the row where the Cell is located.
/// \param column [in] is the column where the Cell is located.
///
void Cell::setCoordinate(rowType row, columnType column)
{
  setRow(row);
  setColumn(column);

  setColor(false);
}

///
/// \brief Cell::setColor() styles the Cell, whether highlighted or not.
/// \param highLight [in] indicates whether the Cell style should be highlighted or not.
///
void Cell::setColor(bool highLight)
{
  // Remove current stylesheet effects.
  style()->unpolish(this);

  QString s = styleSheet();                       // This line retrieves your current StyleSheet for THIS widget.
  StyleSheetProcessor::splitStyleMapType map;     // Creates a map to store your stylesheet elements
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
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, brownColor);    // Replaces the stylesheet element for background color with "brownColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, beigeColor);        // Replaces the stylesheet element for border color with "beigeColor"
    }
    else { // columns  1, 3, 5, and 7
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, beigeColor);    // Replaces the stylesheet element for background color with "beigeColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, brownColor);        // Replaces the stylesheet element for border color with "brownColor"
    }
  }
  // And all uneven row uneven column cells will be brown with beige borders, and all
  // uneven row even column cells will be beige with brown borders.
  else if (_row % 2 != 0) { // rows 1, 3, 5, and 7
    if (_column % 2 != 0) {// columns  1, 3, 5, and 7
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, brownColor);    // Replaces the stylesheet element for background color with "brownColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, beigeColor);        // Replaces the stylesheet element for border color with "beigeColor"
    }
    else { // columns 2, 4, 6, and 8
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBackGroundColor, beigeColor);    // Replaces the stylesheet element for background color with "beigeColor"
      StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderColor, brownColor);        // Replaces the stylesheet element for border color with "brownColor"
    }
  }

  // Solid boder style flattens the button, so it doesn't look like, well, a button.
  StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderStyle, QString("solid"));       // Replaces the stylesheet element for border style with "solid"
  StyleSheetProcessor::addOrReplaceItemInMap(map, StyleSheetProcessor::sBorderWidth, QString("1px"));         // Replaces the stylesheet element for border width with "1px"

  s = StyleSheetProcessor::mergeStyleSheet(map);                                                              // Merges the stylesheet back into a single string.
  setStyleSheet(s);        // Sets the stylesheet of THIS widget.
  style()->polish(this);   // Adds back stylesheet effects.
}

///
/// \brief Cell::assignPiece() does what its name suggests.  It assigns a Chess Piece to this Cell.
/// \param piece [in] is a shared pointer to a definedPieceType Chess Piece.
///
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

///
/// \brief Cell::assignedPiece() is an accessor method to obtain a reference to the Cell's current piece.
/// \return a QSharedPointer<Piece> reference that points the this Cell's assigned piece.
///
QSharedPointer<Piece>& Cell::assignedPiece()
{
  return _myPiece;
}

///
/// \brief Cell::position() is a combination accessor the returns row and column in a complex data type.
/// \return a boardCoordinateType value of the current row and column.
///
boardCoordinateType Cell::position()
{
  return boardCoordinateType(row(), column());
}

///
/// \brief Cell::clearAssignedPiece() clears current assigned piece, reconstructs with Identity:none and Color:none.
/// If _myPiece held the last reference to the shared pointer, a call to this function would also delete the
/// memory that _myPiece used to point to.
///
void Cell::clearAssignedPiece()
{
  _myPiece.clear();
  _myPiece = QSharedPointer<Piece>(new Piece);
  setIcon(QPixmap());
}
