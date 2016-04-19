/**
 * \file   PixmapWidget.cpp
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains the inner management features of a PixmapWidget
 *
 * In this cpp file is housed the functionality to construct and manage a Pixelmap on top of a QWidget
 */

#include "PixmapWidget.h"

#include <QPixmap>
#include <QPainter>
#include <QDebug>

///
/// PixmapWidget::PixmapWidget is the default constructor of the PixmapWidget class.
/// \param parent is the QObject that will eventually destroy the Piece. If it is not null.
///
PixmapWidget::PixmapWidget(QWidget *parent) :
  QWidget(parent),
  _pixmap( )
{

}

///
/// PixmapWidget::~PixmapWidget is the default destructor of PixmapWidget objects
///
PixmapWidget::~PixmapWidget()
{

}

///
/// PixmapWidget::setPixmap applies and scales a Pixmap onto the QWidget
/// \param pm is the QPixmap that is to be painted on the QWidget
///
void PixmapWidget::setPixmap( const QPixmap &pm )
{
  if (!pm.isNull())
  {
    _pixmap = pm;
    updateScaledPixmap();
    repaint();
  }
}

///
/// PixmapWidget::paintEvent handles the call to repaint() and draws the Pixmap
///
void PixmapWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  // Draw the scaled Image centered in the contents rectangle.
  p.drawPixmap( _targetRect, _scaledPixmap );
  p.end();
}

///
/// PixmapWidget::resizeEvent handles resize events to ensure the Pixmap is scaled correctly.
///
void PixmapWidget::resizeEvent(QResizeEvent *)
{
  updateScaledPixmap();
}

///
/// PixmapWidget::updateScaledPixmap is responsible for scaling and centering the Pixmap
///
void PixmapWidget::updateScaledPixmap()
{
  // Scale the image while mainting aspect ratio to fit into the contents rectangle.
  _scaledPixmap = _pixmap.scaled( contentsRect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

  // Draw the scaled Image centered in the contents rectangle.
  _targetRect = _scaledPixmap.rect();
  _targetRect.moveCenter( contentsRect().center() );
}
