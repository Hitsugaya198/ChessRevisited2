///
/// \file   StyleSheetProcessor.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains functionality that enables a developer to quickly switch out
/// style elements like colour and borderstyles by calling a few simple functions,
/// instead of manipulating the stylesheet string manually.
///

#include "StyleSheetProcessor.h"
#include <QStringList>
#include <QDebug>

static QStringList slTypes = QStringList();

StyleSheetProcessor::StyleSheetProcessor(QObject* parent) :
  QObject(parent)
{
  if (slTypes.isEmpty()) {
    initList();
  }
}

StyleSheetProcessor::~StyleSheetProcessor()
{

}

void StyleSheetProcessor::initList()
{
  slTypes.insert(0, QString("background-color"));
  slTypes.insert(1, QString("color"));
  slTypes.insert(2, QString("border-color"));
  slTypes.insert(3, QString("border-style"));
  slTypes.insert(4, QString("border-width"));
}

splitStyleMapType StyleSheetProcessor::splitStyleSheet(QString styleSheet)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  splitStyleMapType styleMap;
  if (!styleSheet.isEmpty()) {
    QStringList properties = styleSheet.split(";", QString::SkipEmptyParts);
    QListIterator<QString> i(properties);

    while (i.hasNext()) {
      QString property = i.next();
      QStringList elements = property.split(":", QString::SkipEmptyParts);
      styleMap.insert(getType(elements.at(0).simplified()), elements.at(1).simplified());
    }
  }
  return styleMap;
}

QString StyleSheetProcessor::mergeStyleSheet(splitStyleMapType& styleMap)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  splitStyleMapType::iterator i;
  QString styleSheet;
  for (i = styleMap.begin(); i != styleMap.end(); ++i) {
    styleSheet += getTypeString(i.key()) + QString(":") + i.value() + QString(";");
  }
  return styleSheet;
}

void StyleSheetProcessor::addOrReplaceItemInMap(splitStyleMapType& styleMap, const eStyleComponents valueType, const QString newValue)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  bool foundIt = false;
  if (!styleMap.isEmpty()) {
    splitStyleMapType::iterator i;
    for (i = styleMap.begin(); i != styleMap.end(); ++i) {
      if (i.key() == valueType) {
        styleMap.remove(i.key());
        styleMap.insert(valueType, newValue);
        foundIt = true;
        break;
      }
    }
    if (!foundIt) {
      styleMap.insert(valueType, newValue);
    }
  }
  else {
    styleMap.insert(valueType, newValue);
  }
}

bool StyleSheetProcessor::isBackGroundColor(QString& input)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  return (input.compare(slTypes.at(sBackGroundColor), Qt::CaseInsensitive) == 0);
}

bool StyleSheetProcessor::isTextColor(QString& input)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  return (input.compare(slTypes.at(sTextColor), Qt::CaseInsensitive) == 0);
}

bool StyleSheetProcessor::isBorderColor(QString& input)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  return (input.compare(slTypes.at(sBorderColor), Qt::CaseInsensitive) == 0);
}

bool StyleSheetProcessor::isBorderStyle(QString& input)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  return (input.compare(slTypes.at(sBorderStyle), Qt::CaseInsensitive) == 0);
}

bool StyleSheetProcessor::isBorderWidth(QString& input)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  return (input.compare(slTypes.at(sBorderWidth), Qt::CaseInsensitive) == 0);
}

eStyleComponents StyleSheetProcessor::getType(QString input)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  if (isBackGroundColor(input)) {
    return sBackGroundColor;
  }
  if (isTextColor(input)) {
    return sTextColor;
  }
  if (isBorderColor(input)) {
    return sBorderColor;
  }
  if (isBorderStyle(input)) {
    return sBorderStyle;
  }
  if (isBorderWidth(input)) {
    return sBorderWidth;
  }
  return sBackGroundColor;
}

QString StyleSheetProcessor::getTypeString(eStyleComponents type)
{
  if (slTypes.isEmpty()) {
    initList();
  }
  return slTypes.at(type);
}
