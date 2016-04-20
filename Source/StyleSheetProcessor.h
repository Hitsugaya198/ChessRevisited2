///
/// \file   StyleSheetProcessor.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains functions outlines that can be used to manipulate
/// Qt stylesheets in a limited but effective way.
///

#ifndef STYLESHEETPROCESSOR_H
#define STYLESHEETPROCESSOR_H

#include "CommonTypeDefinitions.h"

#include <QObject>
#include <QMap>
#include <QStyle>

///
/// The StyleSheetProcessor class
///
class StyleSheetProcessor : public QObject
{

  Q_OBJECT

public:

  ///
  /// StyleSheetProcessor is the default constructor for the stylesheet processor class
  /// \param parent [in] is the QObject that would destroy instances of StyleSheetProcessor, if it is not null.
  ///
  explicit StyleSheetProcessor(QObject* parent = 0);

  ///
  /// ~StyleSheetProcessor is the default destructor of StyleSheetProcessor objects.
  ///
  virtual ~StyleSheetProcessor();

  ///
  /// splitStyleSheet splits the stylesheet into defined map elements.
  ///
  /// A stylesheet is just a formatting string, and not very useful or accessible
  /// in its native form.  This easily expandible class splits it into
  /// a map of single element strings that are easier to replace in-line.
  ///
  /// \param styleSheet [in] is the string that represents the stylesheet to be manipulated.
  /// \return a splitStyleMapType of stylesheet elements.
  ///
  static splitStyleMapType splitStyleSheet(QString styleSheet);

  ///
  /// mergeStyleSheet is the reverse process of splitStyleSheet().
  ///
  /// When done manipulating the map, mergeStyleSheet() merges the
  /// map back into a string that Qt will accept.
  ///
  /// \param styleMap [in] is the splitStyleMapType variable that contains the manipulated stylesheet components.
  /// \return the stylesheet in string format.
  ///
  static QString mergeStyleSheet(splitStyleMapType& styleMap);

  ///
  /// addOrReplaceItemInMap can replace style items in the map, and if they don't exist, they are added to the map.
  ///
  /// \param styleMap [in,out] is the map that must be modified.
  /// \param valueType [in] is the key to use when accessing the style map.
  /// \param newValue [in] is the new value that is to be stored for style component valueType.
  ///
  static void addOrReplaceItemInMap(splitStyleMapType& styleMap, const eStyleComponents valueType, const QString newValue);

  ///
  /// isBackGroundColor parses a string to determine whether it represents background colour.
  ///
  /// \param input [in] the string to be parsed.
  /// \return true if the representation is as expected, false if not.
  ///
  static bool isBackGroundColor(QString& input);

  ///
  /// isTextColor parses a string to determine whether it represents text colour.
  ///
  /// \param input [in] the string to be parsed.
  /// \return true if the representation is as expected, false if not.
  ///
  static bool isTextColor(QString& input);

  ///
  /// isBorderColor parses a string to determine whether it represents border colour.
  ///
  /// \param input [in] the string to be parsed.
  /// \return true if the representation is as expected, false if not.
  ///
  static bool isBorderColor(QString& input);

  ///
  /// isBorderStyle parses a string to determine whether it represents border style.
  ///
  /// \param input [in] the string to be parsed.
  /// \return true if the representation is as expected, false if not.
  ///
  static bool isBorderStyle(QString& input);

  ///
  /// isBorderWidth parses a string to determine whether it represents border width.
  ///
  /// \param input [in] the string to be parsed.
  /// \return true if the representation is as expected, false if not.
  ///
  static bool isBorderWidth(QString& input);

  ///
  /// getType parses a string to determine by the stylesheet tag what enum type it is.
  /// \param input [in] the string that is to be parsed.
  /// \return the enum type value that represents the stylesheet tag indicated by the string.
  ///
  static eStyleComponents getType(QString input);

  ///
  /// getTypeString does the reverse of getType(). It takes the enum type and returns its String representation.
  ///
  /// \param type [in] the eStyleComponents enum element that is to be evaluated.
  /// \return the string that represents the Qt StyleSheet tag for the eStyleComponents value passed in.
  ///
  static QString getTypeString(eStyleComponents type);

private:

  ///
  /// initList initializes the list of stylesheet tags to be used when parsing input.
  ///
  static void initList();
};

#endif // STYLESHEETPROCESSOR_H
