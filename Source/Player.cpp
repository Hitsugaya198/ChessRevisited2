#include "Player.h"

Player::Player(QObject* parent) :
  QObject(parent)
{

}

Player::Player(UserIdentity::eIdentities identity, QObject* parent) :
  QObject(parent)
{
  _identity = identity;
}

// Useless
Player::~Player()
{
  _identity = UserIdentity::eNone;
}

UserIdentity::eIdentities Player::identity() const
{
  return _identity;
}

void Player::setIdentity(const UserIdentity::eIdentities& identity)
{
  _identity = identity;
}


