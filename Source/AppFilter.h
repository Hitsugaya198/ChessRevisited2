#ifndef APPFILTER_H
#define APPFILTER_H

#include <QObject>

class AppFilter : public QObject
{
  public:
    AppFilter(QObject* parent = 0);
    virtual ~AppFilter();

protected:
  bool eventFilter(QObject* obj, QEvent* event);
};

#endif // APPFILTER_H
