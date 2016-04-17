#ifndef CAPTUREDPIECEWIDGET_H
#define CAPTUREDPIECEWIDGET_H

#include <QWidget>

namespace Ui {
  class CapturedPieceWidget;
}

class CapturedPieceWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit CapturedPieceWidget(const QPixmap& pixmap, QWidget *parent = 0);
    ~CapturedPieceWidget();
  void setPixmap (const QPixmap& pixmap);

  private:
    Ui::CapturedPieceWidget *ui;
};

#endif // CAPTUREDPIECEWIDGET_H
