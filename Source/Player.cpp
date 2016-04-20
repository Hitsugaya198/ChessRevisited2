///
/// \file   Player.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the inner management features of a Player.
///
/// In this cpp file is housed the functionality to construct and manage a Player entity.
///

#include "Player.h"

Player::Player(QObject* parent) :
  QObject(parent)
{
  _identity = UserIdentity::eNone;
  _associatedColor = PieceColors::eNone;
}

Player::Player(UserIdentity::eIdentities identity, PieceColors::ePieceColors color, QObject* parent) :
  QObject(parent)
{
  _identity = identity;
  _associatedColor = color;
}

Player::~Player()
{

}

UserIdentity::eIdentities Player::identity() const
{
  return _identity;
}

PieceColors::ePieceColors Player::associatedColor() const
{
  return _associatedColor;
}



