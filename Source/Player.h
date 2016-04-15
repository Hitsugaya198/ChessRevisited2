#ifndef PLAYER_H
#define PLAYER_H

#include "UserIdentity.h"

#include <QObject>

class Player : public QObject
{
    Q_OBJECT
  public:
    explicit Player(QObject *parent = 0);
    explicit Player(UserIdentity::eIdentities identity, QObject *parent = 0);
  virtual ~Player();

    UserIdentity::eIdentities identity() const;
    void setIdentity(const UserIdentity::eIdentities& identity);

  signals:

  public slots:

  private:
    UserIdentity::eIdentities _identity;
};

#endif // PLAYER_H
