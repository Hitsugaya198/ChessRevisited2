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
  class Chess;
}

class Chess : public QMainWindow
{
  Q_OBJECT

public:
  explicit Chess(QWidget* parent = 0);
  virtual ~Chess();

private slots:
  void on_action_New_Game_triggered();
  void on_actionE_xit_triggered();

  void startNewGame();
  void endGame(bool checkMate);

  void updateCapturedPieces();

private:
  Ui::Chess* ui;

  QSharedPointer<Player> _player1;
  QSharedPointer<Player> _player2;
  QScopedPointer<MoveGenerator> _artificialIntelligence;
  QList<CapturedPieceWidget> _blackPieces;
  QList<CapturedPieceWidget> _whitePieces;
  void clearLayout(QLayout* layout);

  QScrollArea* _blackScrollArea;
  QScrollArea* _whiteScrollArea;

  QWidget* _blackContainer;
  QWidget* _whiteContainer;

  QVBoxLayout* _blackLayout;
  QVBoxLayout* _whiteLayout;
};

#endif // CHESS_H
