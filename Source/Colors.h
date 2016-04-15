#ifndef COLORS_H
#define COLORS_H

#include <QString>

struct Colors {
public:

  enum eCellColors {
    eBrown,
    eBeige,
    eInit  = eBrown,
    eMax   = eBeige
  };

  static Colors& getInstance()
  {
    static Colors instance;
    return instance;
  }

  QString getBrown()
  {
    return BROWN;
  }
  QString getBeige()
  {
    return BEIGE;
  }
  QString getHiBrown()
  {
    return HI_BROWN;
  }
  QString getHiBeige()
  {
    return HI_BEIGE;
  }

private:
  Colors()
  {
    BROWN = QString("#663300");
    BEIGE = QString("#EDC25E");
    HI_BEIGE = QString("#F5DCA3");
    HI_BROWN = QString("#994D00");
  }

  Colors(Colors const&);
  void operator=(Colors const&);

  QString BEIGE;
  QString BROWN;
  QString HI_BEIGE;
  QString HI_BROWN;
};

#endif // COLORS_H
