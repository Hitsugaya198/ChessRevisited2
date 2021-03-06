///
/// \file   Cell.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the inner class and member definitions of the Cell class.
///

#ifndef CELL_H
#define CELL_H

#include "CommonTypeDefinitions.h"

#include <QPushButton>
#include <QTimer>
#include <QThread>

///
/// Forward declaration of class Piece
///
class Piece;

///
/// The Cell class is in actual fact just a QPushButton with some added attributes to tailor it as a Cell on a Chess Board.
///
class Cell : public QPushButton
{

  Q_OBJECT

public:
  ///
  /// Cell The default constructor for a Cell object.
  ///
  /// \param parent [in] represents the object that will destroy instances of Cell, if not null. The Cells should have a Board as their parent.
  ///
  explicit Cell(QWidget* parent = 0);

  ///
  /// ~Cell The default destructor that destroys Cell objects.
  ///
  virtual ~Cell();

  ///
  /// row is the accessor function that provides the row in which this Cell is located.
  ///
  /// \return the row number where this Cell resides.
  ///
  rowType row() const;

  ///
  /// column is the accessor function that provides the column in which this Cell is located.
  ///
  /// \return the column number where this Cell resides.
  ///
  columnType column() const;

  ///
  /// assignedPiece is an accessor method to obtain a reference to the Cell's current piece.
  ///
  /// \return a QSharedPointer<Piece> reference that points the this Cell's assigned piece.
  ///
  QSharedPointer<Piece>& assignedPiece();

  ///
  /// position is a combination accessor the returns row and column in a complex data type.
  ///
  /// \return a boardCoordinateType value of the current row and column.
  ///
  boardCoordinateType position();

  ///
  /// setRow is the mutator function that manipulates the row in which this Cell is located.
  ///
  /// \param row [in] the row number to set for this Cell.
  ///
  void setRow(rowType row);

  ///
  /// setColumn is the mutator function that manipulates the column in which this Cell is located.
  ///
  /// \param column [in] the column number to set for this Cell.
  ///
  void setColumn(columnType column);

  ///
  /// setCoordinate is a combined mutator function for allocating a Cell to a Board coordinate.
  ///
  /// \param row [in] is the row where the Cell is located.
  /// \param column [in] is the column where the Cell is located.
  ///
  void setCoordinate(rowType row, columnType column);

  ///
  /// assignPiece does what its name suggests.  It assigns a Chess Piece to this Cell.
  ///
  /// \param piece [in] is a shared pointer to a definedPieceType Chess Piece.
  ///
  void assignPiece(QSharedPointer<Piece> piece);

  ///
  /// clearAssignedPiece clears current assigned piece, reconstructs with Identity:none and Color:none.
  ///
  /// If _myPiece held the last reference to the shared pointer, a call to this function would also delete the
  /// memory that _myPiece used to point to.
  ///
  void clearAssignedPiece();

  ///
  /// toggleCellHighlights changes the color of the Cell between highlighted and regular.
  ///
  /// This function can also be used to color cells by calling \code cell->highLightCell(false); \endcode.
  /// The reason for this is that highlights are merely a lighter shade of the normal color of the Cell,
  /// and not highlighting it is the same as changing it's color to a darker shade of the highlighted color.
  /// The actual color of the Cell is determined by its position on the board.
  ///
  /// \param highLight [in] indicates whether to highlight the Cell or not.
  ///
  void toggleCellHighlights(bool highLight);

  ///
  /// resetCheckedCounter is used to reset the checked counter of all Cell objects back to zero (static member).
  ///
  static void resetCheckedCounter();

private slots:

  ///
  /// handleCellToggled executes when a Cell is toggled (checked or unchecked).
  ///
  /// \param checked [in] indicates whether the cell is being selected or unselected.
  ///
  void handleCellToggled(bool checked);

private:
  ///
  /// _row is the private member that stores the row where the Cell resides.
  ///
  rowType _row;

  ///
  /// _column is the private member that stores the column where the Cell resides.
  ///
  columnType _column;

  ///
  /// _myPiece is the private member that stores the Piece that is allocated to this Cell.
  ///
  QSharedPointer<Piece> _myPiece;

  ///
  /// _checkedCounter is the private member that keeps track of whether or not another Cell was checked before this Cell.
  ///
  static int _checkedCounter;

  ///
  /// setColor styles the Cell, whether highlighted or not.
  ///
  /// \param highLight [in] indicates whether the Cell style should be highlighted or not.
  ///
  void setColor(bool highLight = false);

  ///
  /// checkAccessInit checks the location access of move origin Cell objects.
  ///
  /// \return true if the Player has access to the origin Cell, false if not.
  ///
  bool checkAccessInit();

  ///
  /// checkAccessContinue checks the location access of move destination Cell objects.
  ///
  /// \return true if the moving piece has access to the destination Cell, false if not.
  ///
  bool checkAccessContinue();

signals:

  ///
  /// startingANewMove connects and emits to the Board to let it know a new move is being made.
  ///
  /// \param positionFrom is the coordinate of the Cell that is emitting the signal.
  ///
  void startingANewMove(boardCoordinateType positionFrom);

  ///
  /// completingMove connects and emits to the Board to let it know a move is being completed.
  ///
  /// \param positionTo is the coordinate of the Cell that is emitting the signal.
  ///
  void completingMove(boardCoordinateType positionTo);

  ///
  /// nothingToDo allows clearing of highlights during odd clicking behaviour from the human Player.
  ///
  /// This should not happen too much under ideal circumstances, but it is handled nonetheless.
  ///
  void nothingToDo();
};

#endif // CELL_H
