#ifndef CELL_H
#define CELL_H

#include <QPushButton>
#include <QMutex>
#include <QTimer>
#include <QThread>

#ifndef nullptr
  #define nullptr NULL
#endif

class Piece;

class Cell : public QPushButton
{
  Q_OBJECT
public:
  // Constructor / Destructor
  explicit Cell(QWidget* parent = 0);
  virtual ~Cell();

  typedef int rowType;
  typedef int columnType;
  typedef QPair < rowType, columnType > boardCoordinateType;

  // Public member functions (Accessors)
  int row() const;
  int column() const;
  QSharedPointer<Piece>& assignedPiece();
  boardCoordinateType position();

  // Public member functions (Mutators)
  void setRow(int row);
  void setColumn(int column);
  void setCoordinate(int row, int column);
  void assignPiece(QSharedPointer<Piece> piece);
  void clearAssignedPiece();
  void highLightCell(bool highLight);
  static void resetCheckedCounter();

  // private slots
private slots:
  void handleCellToggled(bool checked);

private:
  // Private data members
  int _row;
  int _column;
  QSharedPointer<Piece> _myPiece;
  Cell* _me;
  QMutex _setColorMutex;
  QMutex _setCoordinateMutex;
  QMutex _assignPieceMutex;
  QMutex _clearAssignPieceMutex;

  // Static private members
  static int _checkedCounter;

  // Private member functions
  void setColor(bool highLight = false);

  bool checkAccessInit();
  bool checkAccessContinue();
  signals:
  void startingANewMove(boardCoordinateType positionFrom);
  void completingMove(boardCoordinateType positionTo);
  void nothingToDo();
};

#endif // CELL_H
