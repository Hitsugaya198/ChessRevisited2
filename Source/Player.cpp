#include "Player.h"

///
/// \brief Player::Player
/// \param parent
///
Player::Player(QObject* parent) :
  QObject(parent)
{
  _identity = UserIdentity::eNone;
}

///
/// \brief Player::Player
/// \param identity
/// \param parent
///
Player::Player(UserIdentity::eIdentities identity, QObject* parent) :
  QObject(parent)
{
  _identity = identity;
}

///
/// \brief Player::~Player
///
Player::~Player()
{

}

///
/// \brief Player::identity
/// \return
///
UserIdentity::eIdentities Player::identity() const
{
  return _identity;
}

///
/// \brief Player::setIdentity
/// \param identity
///
void Player::setIdentity(const UserIdentity::eIdentities& identity)
{
  _identity = identity;
}


