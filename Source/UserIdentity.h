///
/// \file   UserIdentity.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains the inner structure definition for a User's Identity, used for turn management etc.
///

#ifndef USERIDENTITY
#define USERIDENTITY

#include <QObject>

///
/// The UserIdentity struct is just an excuse to wrap an enum in a class.
/// It is useful, if one needs to add manipulations or properties to a
/// UserIdentity, it can also go in this file.
///
struct UserIdentity : public QObject {

  Q_OBJECT

public:

  ///
  /// The eIdentities enum defines the identity values that a user can have.
  ///
  enum eIdentities {
    eNone,           ///< enum value eNone implies an unitialized identity.
    eHuman,          ///< enum value eHuman refers to a human user.
    eComputer,       ///< enum value eComputer refers to an AI user.
    eInit = eHuman,  ///< enum value eInit refers to the first valid entry.
    eMax = eComputer ///< enum value eMax refers to the last valid entry.
  };

  ///
  /// ~UserIdentity is the default destructor for objects of type UserIdentity.
  ///
  virtual ~UserIdentity() {}

  ///
  /// getInstance returns an instance reference, as the constructor is private.  Singleton Pattern.
  /// \return an instance reference to the static object of struct UserIdentity.
  ///
  static UserIdentity& getInstance()
  {
    static UserIdentity instance;
    return instance;
  }


private:

  ///
  /// UserIdentity is the default constructor for objects of type UserIdentity.
  ///
  /// \param parent is the QObject responsible for destroying heap memory allocated by UserIdentity's constructor.  Can be null.
  ///
  explicit UserIdentity(QObject* parent = 0):
    QObject(parent)
  {

  }

  ///
  /// UserIdentity declared as private, this copy- by-const-reference constructor is now no longer accessible.
  ///
  UserIdentity(UserIdentity const&);

  ///
  /// operator = declared as private, this copy- by-const-reference operator is now no longer accessible.
  ///
  void operator=(UserIdentity const&);
};

#endif // USERIDENTITY
