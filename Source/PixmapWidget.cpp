///
/// \file   PixmapWidget.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file contains the inner management features of a PixmapWidget
///
/// In this cpp file is housed the functionality to construct and manage a Pixelmap on top of a QWidget
///

#include "PixmapWidget.h"

#include <QPixmap>
#include <QPainter>
#include <QDebug>

PixmapWidget::PixmapWidget(QWidget *parent) :
  QWidget(parent),
  _pixmap( )
{

}

PixmapWidget::~PixmapWidget()
{

}

void PixmapWidget::setPixmap( const QPixmap &pixmap )
{
  if (!pixmap.isNull())
  {
    _pixmap = pixmap;
    updateScaledPixmap();
    repaint();
  }
}

void PixmapWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  // Draw the scaled Image centered in the contents rectangle.
  p.drawPixmap( _targetRect, _scaledPixmap );
  p.end();
}

void PixmapWidget::resizeEvent(QResizeEvent *)
{
  updateScaledPixmap();
}

void PixmapWidget::updateScaledPixmap()
{
  // Scale the image while mainting aspect ratio to fit into the contents rectangle.
  _scaledPixmap = _pixmap.scaled( contentsRect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

  // Draw the scaled Image centered in the contents rectangle.
  _targetRect = _scaledPixmap.rect();
  _targetRect.moveCenter( contentsRect().center() );
}
