#ifndef PIXMAPWIDGET_H
#define PIXMAPWIDGET_H

#include <QWidget>

class PixmapWidget : public QWidget
{
  Q_OBJECT

  public:
    explicit PixmapWidget(QWidget *parent = 0);
    virtual ~PixmapWidget();

    void setPixmap(const QPixmap &pm );

  protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

  private:
    void updateScaledPixmap();
    QPixmap pixmap_;
    QPixmap scaledPixmap_;
    QRect targetRect_;

};

#endif // PIXMAPWIDGET_H
