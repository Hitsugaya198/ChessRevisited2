/**
 * \file   PixmapWidget.h
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains structure definitions and members of the PixmapWidget class.
 */

#ifndef PIXMAPWIDGET_H
#define PIXMAPWIDGET_H

#include <QWidget>

/**
 * The PixmapWidget class is a graphics widget that draws QPixmap data onto a QWidget
 */
class PixmapWidget : public QWidget
{

  Q_OBJECT

public:

  /**
   * PixmapWidget is the default constructor of the PixmapWidget class.
   * \param parent is the QObject that will eventually destroy the Piece. If it is not null.
   */
  explicit PixmapWidget(QWidget* parent = 0);

  /**
   * ~PixmapWidget is the default destructor of PixmapWidget objects
   */
  virtual ~PixmapWidget();

  /**
   * setPixmap applies and scales a Pixmap onto the QWidget
   * \param pm is the QPixmap that is to be painted on the QWidget
   */
  void setPixmap(const QPixmap& pm);

protected:

  /**
   * paintEvent handles the call to repaint() and draws the Pixmap
   */
  virtual void paintEvent(QPaintEvent*);

  /**
   * resizeEvent handles resize events to ensure the Pixmap is scaled correctly.
   */
  virtual void resizeEvent(QResizeEvent*);

private:

  /**
   * _pixmap is the private member that contains the pixmap drawn onto the QWidget base.
   */
  QPixmap _pixmap;

  /**
   * _scaledPixmap is the scaled representative of _pixmap.
   */
  QPixmap _scaledPixmap;

  /**
   * _targetRect is the set of coordinates that represent the _scaledPixmap's location and size.
   */
  QRect _targetRect;

  /**
   * updateScaledPixmap is responsible for scaling and centering the Pixmap
   */
  void updateScaledPixmap();
};

#endif // PIXMAPWIDGET_H
