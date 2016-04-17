#include "Chess.h"
#include "ui_Chess.h"

#include "TurnManager.h"
#include "UserIdentity.h"
#include "Board.h"

#include <QMessageBox>

///
/// \brief Chess::Chess
/// \param parent
///
Chess::Chess(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::Chess),
  _player1(new Player(UserIdentity::eHuman)),
  _player2(new Player(UserIdentity::eComputer)),
  _artificialIntelligence(new MoveGenerator())
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
  connect(ui->_theGameBoard, SIGNAL(updateCapturedPiecesSignal()),
          this, SLOT(updateCapturedPieces()));

  on_action_New_Game_triggered();
}

///
/// \brief Chess::~Chess
///
Chess::~Chess()
{
  delete ui;
}

///
/// \brief Chess::on_action_New_Game_triggered
///
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

///
/// \brief Chess::on_actionE_xit_triggered
///
void Chess::on_actionE_xit_triggered()
{

}

///
/// \brief Chess::startNewGame
///
void Chess::startNewGame()
{

}

///
/// \brief Chess::endGame
/// \param checkMate
///
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

///
/// \brief Chess::updateCapturedPieces
///
void Chess::updateCapturedPieces()
{
  QWidget* whitePiecesContainer = ui->_whitePiecesScroller;
  QWidget* blackPiecesContainer = ui->_blackPiecesScroller;

  QLayout* whiteLayout = whitePiecesContainer->layout();
  QLayout* blackLayout = blackPiecesContainer->layout();

  QVBoxLayout* castedWhiteLayout = dynamic_cast<QVBoxLayout*>(whiteLayout);
  QVBoxLayout* castedBlackLayout = dynamic_cast<QVBoxLayout*>(blackLayout);

  QVBoxLayout* newWhiteLayout;
  QVBoxLayout* newBlackLayout;

  if(castedBlackLayout == 0)
  {
    newBlackLayout = new QVBoxLayout();
    delete blackLayout;
  }
  else
  {
    newBlackLayout = castedBlackLayout;
  }

  if(castedWhiteLayout == 0)
  {
    newWhiteLayout = new QVBoxLayout();
    delete whiteLayout;
  }
  else
  {
    newWhiteLayout = castedWhiteLayout;
  }

  clearLayout(newBlackLayout);
  clearLayout(newWhiteLayout);

  piecesListType capturedPieces = ui->_theGameBoard->workingCapturedPieces();
  piecesListType::iterator i = capturedPieces.begin();

  while(i != capturedPieces.end())
  {
    definedPieceType x = *i;
    ++i;

    CapturedPieceWidget* capturedPiece = new CapturedPieceWidget();
    QPixmap pm;
    QString colorString = Pieces::getInstance().colorNames().at(x.second);
    QString identityString = Pieces::getInstance().identityNames().at(x.first);
    QString resPath = QString(":/Pieces/") + QString("Resources/") + colorString + QString("/") + identityString + QString(".png");
    pm.load(resPath, "PNG");
    capturedPiece->setPixmap(pm);

    switch(x.second)
    {
      case Pieces::PieceColors::eBlack:
      {
        newBlackLayout->addWidget(capturedPiece);
        break;
      }
      case Pieces::PieceColors::eWhite:
      {
        newWhiteLayout->addWidget(capturedPiece);
        break;
      }
    }
  }
  whitePiecesContainer->setLayout(newWhiteLayout);
  blackPiecesContainer->setLayout(newBlackLayout);
}

///
/// \brief Chess::clearLayout
/// \param layout
///
void Chess::clearLayout(QLayout *layout)
{
    if (layout) {
        while(layout->count() > 0){
            QLayoutItem *item = layout->takeAt(0);
            delete item->widget();
            delete item;
        }
    }
}
