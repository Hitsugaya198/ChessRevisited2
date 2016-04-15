#include "UserIdentity.h"
#include <QDebug>

UserIdentity::eIdentities UserIdentity::_currentIdentity = eNone;

///
/// \brief UserIdentity::UserIdentity
/// \param parent
///
UserIdentity::UserIdentity(QObject* parent):
  QObject(parent)
{
  setCurrentIdentity(eHuman, false);
}

///
/// \brief UserIdentity::setCurrentIdentity
/// \param currentIdentity
///
UserIdentity::eIdentities UserIdentity::currentIdentity() const
{
  return _currentIdentity;
}

void UserIdentity::switchIdentity()
{


  if (_currentIdentity == eHuman) {
    getInstance().setCurrentIdentity(eComputer);
  }
  else {
    getInstance().setCurrentIdentity(eHuman);
  }
}

void UserIdentity::setCurrentIdentity(const UserIdentity::eIdentities& currentIdentity, bool notify)
{
  _currentIdentity = currentIdentity;
  if (notify) {
    emitSignalInThread();
  }

}

void UserIdentity::emitSignalInThread()
{
  emit identityChanged();
}
