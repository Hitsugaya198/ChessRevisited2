///
/// \file   StyleComponents.h
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
///
/// This file contains typedef data types used for styling the Chess Board Cells.
///

#ifndef STYLECOMPONENTS
#define STYLECOMPONENTS

///
/// The eStyleComponents enum
///
enum eStyleComponents {
sBackGroundColor = 0,///< enum value sBackGroundColor refers to the background colour style element of a stylesheet.
sTextColor,          ///< enum value sTextColor refers to the text colour style element of a stylesheet.
sBorderColor,        ///< enum value sBorderColor refers to the border colour style element of a stylesheet.
sBorderStyle,        ///< enum value sBorderStyle refers to the border style style element of a stylesheet.
sBorderWidth,        ///< enum value sBorderWidth refers to the border width style element of a stylesheet.
sMaxComponents       ///< enum value sMaxComponents is the maximum number of style elements handled by StyleSheetProcessor.
};

#endif // STYLECOMPONENTS

