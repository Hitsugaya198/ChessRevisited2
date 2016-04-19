///
/// \file   Player.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file contains structure definitions and members of the Player class.
///

#ifndef PLAYER_H
#define PLAYER_H

#include "UserIdentity.h"
#include "Pieces.h"
#include "Colors.h"

#include <QObject>

///
/// The Player class represents the structure and functionality of a Chess Player entity
///
class Player : public QObject
{

  Q_OBJECT

public:

  ///
  /// Player::Player is the default constructor for class Player that creates "blank" Players.
  ///
  /// \param parent is the QObject that will eventually destroy the Piece. If it is not null.
  ///
  explicit Player(QObject* parent = 0);

  ///
  /// Player::Player is a specialized constructor for class Player that creates real Players.
  ///
  /// \param identity [in] the identity of the Player being created.
  /// \param color [in] the colour of pieces associated with the Player being created.
  /// \param parent is the QObject that will eventually destroy the Piece. If it is not null.
  ///
  explicit Player(UserIdentity::eIdentities identity, PieceColors::ePieceColors color, QObject* parent = 0);

  ///
  /// Player::~Player the default destructor of Player objects.
  ///
  virtual ~Player();

  ///
  /// Player::identity is an accessor method that provides access to the Player's identity.
  ///
  /// \return the identity of the current Player as defined in \file UserIdentity.h
  ///
  UserIdentity::eIdentities identity() const;

  ///
  /// Player::associatedColor is an accessor method that provides access to the associated Piece colour for this Player.
  ///
  /// \return the colour associated with this Player, as defined in \file Colors.h.
  ///
  PieceColors::ePieceColors associatedColor() const;

signals:

public slots:

private:

  ///
  /// _identity is the member that stores the identity of the this Player.
  ///
  UserIdentity::eIdentities _identity;

  ///
  /// _associatedColor is the member that stores the colour associated with this Player.
  ///
  PieceColors::ePieceColors _associatedColor;
};

#endif // PLAYER_H
