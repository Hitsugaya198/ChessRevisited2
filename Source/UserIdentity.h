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



  ~UserIdentity() {}

  static UserIdentity& getInstance()
  {
    static UserIdentity instance;
    return instance;
  }

  eIdentities currentIdentity() const;
//  {
//    return _currentIdentity;
//  }



  static void switchIdentity();

  void setCurrentIdentity(const eIdentities& currentIdentity, bool notify = true);

signals:
  static void identityChanged();

private:
  explicit UserIdentity(QObject* parent = 0);

  UserIdentity(UserIdentity const&);
  void operator=(UserIdentity const&);

  static eIdentities _currentIdentity;
  static void emitSignalInThread();
};

#endif // USERIDENTITY
