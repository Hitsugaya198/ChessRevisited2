/**
 * @file   AppFilter.cpp
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file contains the QObject override for function eventFilter()
 */

#include "AppFilter.h"

#include <QEvent>

///
/// \brief AppFilter::AppFilter() is the default constructor for AppFilter objects.
/// \param parent is the QObject that will eventually destroy the pointer to AppFilter (if not null).
///
AppFilter::AppFilter(QObject *parent)
  : QObject(parent)
{

}

///
/// \brief AppFilter::~AppFilter() is the default destructor for AppFilter objects.
///
AppFilter::~AppFilter()
{

}

///
/// \brief AppFilter::eventFilter() filters events if this object has been installed as an event filter for the watched object.
/// \param watched the Object that the event filter has been installed on.
/// \param event is any event that took place on the given object, like keystrokes, mouse activity etc.
/// \return true if an event should not be handled further. False if the event should be processed.
///
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

