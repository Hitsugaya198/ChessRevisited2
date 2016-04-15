#include "Chess.h"
#include "ui_Chess.h"

Chess::Chess(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::Chess)
{
  ui->setupUi(this);
}

Chess::~Chess()
{
  delete ui;
}

void Chess::on_action_New_Game_triggered()
{

}

void Chess::on_actionE_xit_triggered()
{

}

void Chess::startNewGame()
{

}

void Chess::endGame(bool checkMate)
{

}
