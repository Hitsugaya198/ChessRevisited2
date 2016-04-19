///
/// \file   AppFilter.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file contains the QObject override for function eventFilter()
///

#include "AppFilter.h"

#include <QEvent>

AppFilter::AppFilter(QObject *parent)
  : QObject(parent)
{

}

AppFilter::~AppFilter()
{

}

bool AppFilter::eventFilter(QObject* watched, QEvent* event)
{
  // Consider events that need to be blocked when the AI is making a move
  switch (event->type()) {

  case QEvent::KeyPress:
  case QEvent::KeyRelease:
  case QEvent::MouseButtonRelease:
  case QEvent::MouseButtonPress:
  case QEvent::MouseButtonDblClick:
    //...
    return true; // Return true to indicate an event must be filtered out
  }

  // Do what the QObject eventFilter function would have done anyway.
  return QObject::eventFilter(watched, event);
}

