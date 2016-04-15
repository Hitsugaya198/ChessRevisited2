#ifndef USERIDENTITY
#define USERIDENTITY

#include <QObject>

struct UserIdentity : public QObject {

  Q_OBJECT

public:

  enum eIdentities {
    eNone,
    eHuman,
    eComputer,
    eInit = eHuman,
    eMax = eComputer
  };



virtual ~UserIdentity() {}

  static UserIdentity& getInstance()
  {
    static UserIdentity instance;
    return instance;
  }


private:
  explicit UserIdentity(QObject* parent = 0);

  UserIdentity(UserIdentity const&);
  void operator=(UserIdentity const&);

};

#endif // USERIDENTITY
