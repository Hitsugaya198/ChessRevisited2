#include "AppFilter.h"

#include <QEvent>

AppFilter::AppFilter(QObject *parent)
  : QObject(parent)
{

}

AppFilter::~AppFilter()
{

}

bool AppFilter::eventFilter(QObject* obj, QEvent* event)
{
  switch (event->type()) {

  case QEvent::KeyPress:
  case QEvent::KeyRelease:
  case QEvent::MouseButtonRelease:
  case QEvent::MouseButtonPress:
  case QEvent::MouseButtonDblClick:
    //...
    return true;
  }
  return QObject::eventFilter(obj, event);
}

