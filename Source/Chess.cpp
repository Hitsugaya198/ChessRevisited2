/**
 * \file   Chess.cpp
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains the inner management features of Chess game
 *
 * In this cpp file is housed all the functions and attributes needed to construct and manage a
 * a Chess game.
 *
 * Chess connects the various elements of the game together, and manages the graphical aspects of the application.
 */

#include "Chess.h"
#include "ui_Chess.h"

#include "TurnManager.h"
#include "UserIdentity.h"
#include "Board.h"
#include "MoveMapper.h"

#include <QMessageBox>

///
/// Chess::Chess() is the default constructor for the MainWindow of the application.
/// \param parent is always zero for this class, as it is the main entry point of the application.
/// \code
///   Chess w;
///   w.show();
/// \endcode
///
/// This construct also initializes the user interface class Ui::Chess, as well as the Player
/// entities that will take part in the game.
///
/// To manage the AI Player, an instance of MoveGenerator is also created.
///
Chess::Chess(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::Chess),
  _humanPlayer(new Player(UserIdentity::eHuman, PieceColors::eWhite)),
  _aiPlayer(new Player(UserIdentity::eComputer, PieceColors::eBlack)),
  _artificialIntelligence(new MoveGenerator())
{
  ui->setupUi(this);

  QPixmap pm;
  pm.load(":/Icons/Resources/chess_logo.png", "PNG");
  QIcon icon = QIcon(pm);
  setWindowIcon(icon);
  ui->_theGameBoard->setHumanPlayer(_humanPlayer);
  ui->_theGameBoard->setAiPlayer(_aiPlayer);

  _artificialIntelligence->associateGameBoard(ui->_theGameBoard);
  _artificialIntelligence->setAiPlayer(_aiPlayer);

  MoveMapper::getInstance().associateGameBoard(ui->_theGameBoard);

  // Lets the AI know that it's now somebody else's turn
  connect(&TurnManager::getInstance(), SIGNAL(turnChanged(QSharedPointer<Player>&, boardCoordinatesType&, bool)),
          _artificialIntelligence.data(), SLOT(handleTurnChange(QSharedPointer<Player>&, boardCoordinatesType&, bool)));

  // Lets the AI know it has to complete its move
  connect(ui->_theGameBoard, SIGNAL(aiMoveCompletionRequired()),
          _artificialIntelligence.data(), SLOT(handleMoveCompletionRequired()));

  // Tells the UI to update the containers of captured pieces (visually).
  connect(ui->_theGameBoard, SIGNAL(updateCapturedPiecesSignal()),
          this, SLOT(updateCapturedPieces()));

  // Allows the game to end.
  connect(&TurnManager::getInstance(), SIGNAL(endGame(bool)),
          this, SLOT(endGame(bool)));

  /* --------- Setup Captured Pieces Display area --------- */
  _blackScrollArea = new QScrollArea(ui->_blackPiecesArea);
  _whiteScrollArea = new QScrollArea(ui->_whitePiecesArea);

  _blackScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  _whiteScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  _blackScrollArea->setWidgetResizable(true);
  _whiteScrollArea->setWidgetResizable(true);

  _blackContainer = new QWidget(_blackScrollArea);
  _whiteContainer = new QWidget(_whiteScrollArea);

  _blackScrollArea->setWidget(_blackContainer);
  _whiteScrollArea->setWidget(_whiteContainer);

  _blackLayout = new QVBoxLayout(_blackContainer);
  _blackContainer->setLayout(_blackLayout);

  _whiteLayout = new QVBoxLayout(_whiteContainer);
  _whiteContainer->setLayout(_whiteLayout);

  QLayout* lay = ui->_blackPiecesArea->layout();
  lay->addWidget(_blackScrollArea);

  lay = ui->_whitePiecesArea->layout();
  lay->addWidget(_whiteScrollArea);
  /* ------------------------------------------------------ */

  // Assuming this is why the application was launched...
  startNewGame();
}

///
/// Chess::~Chess() is the default destructor of Chess objects
///
Chess::~Chess()
{
  delete ui;
}

///
/// Chess::on_action_New_Game_triggered() is the Ui-generated slot for the "New Game" menu option.
///
void Chess::on_action_New_Game_triggered()
{
  startNewGame();
}

///
/// Chess::on_actionE_xit_triggered() is the Ui-generated slot for the "Exit" menu option.
///
void Chess::on_actionE_xit_triggered()
{
  QApplication::exit(0);
}

///
/// Chess::startNewGame() does as its name suggests.
///
/// To start a new game, the board has to be reset, the players are recreated,
/// the game Board is re-enabled, as endGame() disables it, the players are shared with
/// the Board instance, and finally, the TurnManager is informed it is now the human
/// player's turn.
///
void Chess::startNewGame()
{
  ui->_theGameBoard->resetBoard(false, false);
  _humanPlayer.reset(new Player(UserIdentity::eHuman, PieceColors::eWhite));
  _aiPlayer.reset(new Player(UserIdentity::eComputer, PieceColors::eBlack));
  ui->_theGameBoard->setEnabled(true);
  ui->_theGameBoard->setHumanPlayer(_humanPlayer);
  ui->_theGameBoard->setAiPlayer(_aiPlayer);
  TurnManager::getInstance().switchPlayers(_humanPlayer);
}

///
/// Chess::endGame() is the slot that concludes the game flow.
/// \param checkMate indicates whether a King was checked when the game ended.
///
/// The game is always, and only ended when no more moves are possible
/// for the player whose turn it is.  If a King was checked, it's checkmate,
/// if not, it's stalemate.
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
/// Chess::updateCapturedPieces() clears out the old captured pieces and repopulates from the working state.
///
void Chess::updateCapturedPieces()
{
  clearLayout(_blackLayout);
  clearLayout(_whiteLayout);

  piecesListType capturedPieces = ui->_theGameBoard->workingCapturedPieces();
  piecesListType::iterator i = capturedPieces.begin();

  while (i != capturedPieces.end()) {
    definedPieceType x = *i;
    ++i;

    QPixmap pm;
    QString colorString = PieceColors::getInstance().colorNames().at(x.second);
    QString identityString = Pieces::getInstance().identityNames().at(x.first);
    QString resPath = QString(":/Pieces/") + QString("Resources/") + colorString + QString("/") + identityString + QString(".png");
    pm.load(resPath, "PNG");
    CapturedPieceWidget* capturedPiece = new CapturedPieceWidget(pm);

    switch (x.second) {
    case PieceColors::eBlack: {
      _blackLayout->addWidget(capturedPiece);
      break;
    }
    case PieceColors::eWhite: {
      _whiteLayout->addWidget(capturedPiece);
      break;
    }
    }
  }
}

///
/// Chess::clearLayout() is the function used for clearing out the old captured pieces
/// \param layout is the layout to be cleared of all items.
///
void Chess::clearLayout(QLayout* layout)
{
  if (layout) {
    while (layout->count() > 0) {
      QLayoutItem* item = layout->takeAt(0);
      delete item->widget();
      delete item;
    }
  }
}
