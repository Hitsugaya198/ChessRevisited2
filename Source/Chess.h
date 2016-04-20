///
/// \file   Chess.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file contains the inner class and member definitions of the Chess class.
///

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
  ///
  /// Forward declaration of the Ui class Chess
  ///
  class Chess;
}

///
/// The Chess class represents the MainWindow of the application and manages the game flow and graphical elements.
///
class Chess : public QMainWindow
{
  Q_OBJECT

public:

  ///
  /// Chess is the default constructor for the MainWindow of the application.
  ///
  /// This construct also initializes the user interface class Ui::Chess, as well as the Player
  /// entities that will take part in the game.
  ///
  /// To manage the AI Player, an instance of MoveGenerator is also created by the constructor.
  ///
  /// \param parent is always zero for this class, as it is the main entry point of the application.
  ///
  explicit Chess(QWidget* parent = 0);

  ///
  /// ~Chess is the default destructor of Chess objects.
  ///
  virtual ~Chess();

private slots:

  ///
  /// on_action_New_Game_triggered is the Ui-generated slot for the "New Game" menu option.
  ///
  void on_action_New_Game_triggered();

  ///
  /// on_actionE_xit_triggered is the Ui-generated slot for the "Exit" menu option.
  ///
  void on_actionE_xit_triggered();

  ///
  /// startNewGame does as its name suggests.
  ///
  /// To start a new game, the board has to be reset, the Players are recreated,
  /// the game Board is re-enabled, as endGame() disables it, the Players are shared with
  /// the Board instance, and finally, the TurnManager is informed it is now the human
  /// Player's turn.
  ///
  void startNewGame();

  ///
  /// endGame is the slot that concludes the game flow.
  ///
  /// The game is always, and only ended when no more moves are possible
  /// for the Player whose turn it is.  If a King was checked, it's checkmate,
  /// if not, it's stalemate.
  ///
  /// \param checkMate indicates whether a King was checked when the game ended.
  ///
  void endGame(bool checkMate);

  ///
  /// updateCapturedPieces clears out the old captured pieces and repopulates from the working state.
  ///
  void updateCapturedPieces();

private:
  ///
  /// ui is a pointer private member to the Ui::Class instance (the GUI).
  ///
  Ui::Chess* ui;

  ///
  /// _humanPlayer is a shared pointer of type Player that gets assigned to Board class for usage.
  ///
  QSharedPointer<Player> _humanPlayer;

  ///
  /// _aiPlayer is a shared pointer of type Player that gets assigned to Board class for usage.
  ///
  QSharedPointer<Player> _aiPlayer;

  ///
  /// _artificialIntelligence is a scoped pointer of type MoveGenerator.  It governs the AI Player's movements.
  ///
  QScopedPointer<MoveGenerator> _artificialIntelligence;

  ///
  /// _blackPieces is a list that can hold CapturedPieceWidget instances, and is populated as black pieces are captured.
  ///
  QList<CapturedPieceWidget> _blackPieces;

  ///
  /// _whitePieces is a list that can hold CapturedPieceWidget instances, and is populated as white pieces are captured.
  ///
  QList<CapturedPieceWidget> _whitePieces;

  ///
  /// _blackScrollArea is the scroll area where capture black pieces are displayed.
  ///
  QScrollArea* _blackScrollArea;

  ///
  /// _whiteScrollArea  is the scroll area where capture white pieces are displayed.
  ///
  QScrollArea* _whiteScrollArea;

  ///
  /// _blackContainer is the main body widget of the _blackScrollArea.
  ///
  QWidget* _blackContainer;

  ///
  /// _whiteContainer is the main body widget of the _whiteScrollArea.
  ///
  QWidget* _whiteContainer;

  ///
  /// _blackLayout is the layout used to arrange the black pieces.
  ///
  QVBoxLayout* _blackLayout;

  ///
  /// _whiteLayout is the layout used to arrange the white pieces.
  ///
  QVBoxLayout* _whiteLayout;

  ///
  /// clearLayout is the function used for clearing out the old captured pieces.
  ///
  /// \param layout is the layout to be cleared of all items.
  ///
  void clearLayout(QLayout* layout);
};

#endif // CHESS_H
