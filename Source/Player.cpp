#include "Player.h"

///
/// \brief Player::Player
/// \param parent
///
Player::Player(QObject* parent) :
  QObject(parent)
{
  _identity = UserIdentity::eNone;
  _associatedColor = PieceColors::eNone;
}

///
/// \brief Player::Player
/// \param identity
/// \param parent
///
Player::Player(UserIdentity::eIdentities identity, PieceColors::ePieceColors color, QObject* parent) :
  QObject(parent)
{
  _identity = identity;
  _associatedColor = color;
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
/// \brief Player::associatedColor
/// \return
///
PieceColors::ePieceColors Player::associatedColor() const
{
    return _associatedColor;
}



