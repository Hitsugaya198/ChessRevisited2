#include "Chess.h"
#include "ui_Chess.h"

#include "TurnManager.h"
#include "UserIdentity.h"
#include "Board.h"

#include <QMessageBox>

Chess::Chess(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::Chess),
  _player1(new Player(UserIdentity::eHuman)),
  _player2(new Player(UserIdentity::eComputer)),
  _artificialIntelligence (new MoveGenerator())
{
  ui->setupUi(this);

  QPixmap pm;
  pm.load(":/Icons/Resources/chess_logo.png", "PNG");
  QIcon icon = QIcon(pm);
  setWindowIcon(icon);
  ui->_theGameBoard->setPlayer1(_player1);
  ui->_theGameBoard->setPlayer2(_player2);

  _artificialIntelligence->associateGameBoard(ui->_theGameBoard);
  _artificialIntelligence->setAiPlayer(_player2);

  connect(&TurnManager::getInstance(), SIGNAL(turnChanged(QSharedPointer<Player>&)),
          _artificialIntelligence.data(), SLOT(handleTurnChange(QSharedPointer<Player>&)));
  connect(ui->_theGameBoard, SIGNAL(aiMoveCompletionRequired()),
          _artificialIntelligence.data(), SLOT(handleMoveCompletionRequired()));

  on_action_New_Game_triggered();
}

Chess::~Chess()
{
  delete ui;
}

void Chess::on_action_New_Game_triggered()
{
  ui->_theGameBoard->resetBoard(false, false);
  _player1.reset(new Player(UserIdentity::eHuman));
  _player2.reset(new Player(UserIdentity::eComputer));
  ui->_theGameBoard->setEnabled(true);
  ui->_theGameBoard->setPlayer1(_player1);
  ui->_theGameBoard->setPlayer2(_player2);
  TurnManager::getInstance().switchPlayers(_player1);
}

void Chess::on_actionE_xit_triggered()
{

}

void Chess::startNewGame()
{

}

void Chess::endGame(bool checkMate)
{
  ui->_theGameBoard->setEnabled(false);
  if (checkMate) {
    QMessageBox::information(0, QString("Game Over"), QString("Check and mate!"), QMessageBox::Ok);
  }
  else {
    QMessageBox::information(0, QString("Game Over"), QString("The game has gone stale."), QMessageBox::Ok);
  }
}
