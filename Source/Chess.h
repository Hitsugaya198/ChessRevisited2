/**
 * @file   Chess.h
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the inner class and member definitions of the Chess class.
 */

#ifndef CHESS_H
#define CHESS_H

#include "Player.h"
#include "MoveGenerator.h"
#include "CapturedPieceWidget.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui
{
  /**
   * Forward declaration of the Ui class Chess
   */
  class Chess;
}

/**
 * @brief The Chess class represents the MainWindow of the application and manages the game flow and graphical elements.
 */
class Chess : public QMainWindow
{
  Q_OBJECT

public:

  /**
  * @brief Chess is the default constructor for the MainWindow of the application.
  * @param parent is always zero for this class, as it is the main entry point of the application.
  */
  explicit Chess(QWidget* parent = 0);

  /**
  * @brief ~Chess is the default destructor of Chess objects
  */
  virtual ~Chess();

private slots:

  /**
  * @brief on_action_New_Game_triggered is the Ui-generated slot for the "New Game" menu option.
  */
  void on_action_New_Game_triggered();

  /**
   * @brief on_actionE_xit_triggered is the Ui-generated slot for the "Exit" menu option.
   */
  void on_actionE_xit_triggered();

  /**
   * @brief startNewGame does as its name suggests.
   */
  void startNewGame();

  /**
   * @brief endGame is the slot that concludes the game flow.
   * @param checkMate indicates whether a King was checked when the game ended.
   */
  void endGame(bool checkMate);

  /**
   * @brief updateCapturedPieces clears out the old captured pieces and repopulates from the working state.
   */
  void updateCapturedPieces();

private:
  /**
   * @brief ui is a pointer private member to the Ui::Class instance (the GUI).
   */
  Ui::Chess* ui;

  /**
   * @brief _humanPlayer is a shared pointer of type Player that gets assigned to Board class for usage.
   */
  QSharedPointer<Player> _humanPlayer;

  /**
   * @brief _aiPlayer is a shared pointer of type Player that gets assigned to Board class for usage.
   */
  QSharedPointer<Player> _aiPlayer;

  /**
   * @brief _artificialIntelligence is a scoped pointer of type MoveGenerator.  It governs the AI Player's movements.
   */
  QScopedPointer<MoveGenerator> _artificialIntelligence;

  /**
   * @brief _blackPieces is a list that can hold CapturedPieceWidget instances, and is populated as black pieces are captured.
   */
  QList<CapturedPieceWidget> _blackPieces;

  /**
   * @brief _whitePieces is a list that can hold CapturedPieceWidget instances, and is populated as white pieces are captured.
   */
  QList<CapturedPieceWidget> _whitePieces;

  /**
   * @brief _blackScrollArea is the scroll area where capture black pieces are displayed.
   */
  QScrollArea* _blackScrollArea;

  /**
   * @brief _whiteScrollArea  is the scroll area where capture white pieces are displayed.
   */
  QScrollArea* _whiteScrollArea;

  /**
   * @brief _blackContainer is the main body widget of the _blackScrollArea
   */
  QWidget* _blackContainer;

  /**
   * @brief _whiteContainer is the main body widget of the _whiteScrollArea
   */
  QWidget* _whiteContainer;

  /**
   * @brief _blackLayout is the layout used to arrange the black pieces.
   */
  QVBoxLayout* _blackLayout;

  /**
   * @brief _whiteLayout is the layout used to arrange the white pieces.
   */
  QVBoxLayout* _whiteLayout;

  /**
   * @brief clearLayout is the function used for clearing out the old captured pieces
   * @param layout is the layout to be cleared of all items.
   */
  void clearLayout(QLayout* layout);
};

#endif // CHESS_H
