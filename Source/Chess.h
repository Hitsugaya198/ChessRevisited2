#ifndef CHESS_H
#define CHESS_H

#include <QMainWindow>

namespace Ui {
  class Chess;
}

class Chess : public QMainWindow
{
    Q_OBJECT

  public:
    explicit Chess(QWidget *parent = 0);
    ~Chess();

  private slots:
    void on_action_New_Game_triggered();
    void on_actionE_xit_triggered();

    void startNewGame();
    void endGame(bool checkMate);

    QWidget* _oldBoard;


  private:
    Ui::Chess *ui;
};

#endif // CHESS_H
