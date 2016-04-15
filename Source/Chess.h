#ifndef CHESS_H
#define CHESS_H

#include "Player.h"
#include "MoveGenerator.h"

#include <QMainWindow>

namespace Ui {
  class Chess;
}

class Chess : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Chess(QWidget *parent = 0);
  virtual ~Chess();

  private slots:
    void on_action_New_Game_triggered();
    void on_actionE_xit_triggered();

    void startNewGame();
    void endGame(bool checkMate);



  private:
    Ui::Chess *ui;

    QSharedPointer<Player> _player1;
    QSharedPointer<Player> _player2;
    QScopedPointer<MoveGenerator> _artificialIntelligence;

};

#endif // CHESS_H
