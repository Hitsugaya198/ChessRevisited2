#ifndef PLAYER_H
#define PLAYER_H

#include "UserIdentity.h"
#include "Pieces.h"

#include <QObject>

class Player : public QObject
{
  Q_OBJECT
public:
  explicit Player(QObject* parent = 0);
  explicit Player(UserIdentity::eIdentities identity, Pieces::PieceColors::ePieceColors color, QObject* parent = 0);
  virtual ~Player();

  UserIdentity::eIdentities identity() const;
  Pieces::PieceColors::ePieceColors associatedColor() const;

  signals:

  public slots:

  private:
  UserIdentity::eIdentities _identity;
  Pieces::PieceColors::ePieceColors _associatedColor;
};

#endif // PLAYER_H
