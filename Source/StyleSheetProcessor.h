#ifndef STYLESHEETPROCESSOR_H
#define STYLESHEETPROCESSOR_H

#include <QObject>
#include <QMap>
#include <QStyle>

class StyleSheetProcessor : public QObject
{
  Q_OBJECT
public:

  enum eStyleComponents {
    sBackGroundColor = 0,
    sTextColor,
    sBorderColor,
    sBorderStyle,
    sBorderWidth,
    sMaxComponents
  };


  explicit StyleSheetProcessor(QObject* parent = 0);
  virtual ~StyleSheetProcessor();

  typedef QMap<eStyleComponents, QString> splitStyleMapType;
  static splitStyleMapType splitStyleSheet(QString styleSheet);

  static QString mergeStyleSheet(splitStyleMapType& styleMap);

  static void addOrReplaceItemInMap(StyleSheetProcessor::splitStyleMapType& styleMap, const StyleSheetProcessor::eStyleComponents valueType, const QString newValue);

  static bool isBackGroundColor(QString& input);
  static bool isTextColor(QString& input);
  static bool isBorderColor(QString& input);
  static bool isBorderStyle(QString& input);
  static bool isBorderWidth(QString& input);
  static eStyleComponents getType(QString input);
  static QString getTypeString(eStyleComponents type);

private:
  static void initList();

signals:

public slots:

};

#endif // STYLESHEETPROCESSOR_H
