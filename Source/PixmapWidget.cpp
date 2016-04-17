#include "PixmapWidget.h"

#include <QPixmap>
#include <QPainter>
#include <QDebug>

PixmapWidget::PixmapWidget(QWidget *parent) :
  QWidget(parent),
  pixmap_( /*QPixmap( ":/general/General/no_image.png", "PNG" )*/ )
{
//  updateScaledPixmap();
//  setMinimumWidth( 40 );
}

PixmapWidget::~PixmapWidget()
{
}

void PixmapWidget::setPixmap( const QPixmap &pm )
{
  if (!pm.isNull())
  {
    pixmap_ = pm;
    updateScaledPixmap();
    repaint();
  }
}

void PixmapWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  // Draw the scaled Image centered in the contents rectangle.
  p.drawPixmap( targetRect_, scaledPixmap_ );
  p.end();
}

void PixmapWidget::resizeEvent(QResizeEvent *)
{
  updateScaledPixmap();
}

void PixmapWidget::updateScaledPixmap()
{
  // Scale the image while mainting aspect ratio to fit into the contents rectangle.
  scaledPixmap_ = pixmap_.scaled( contentsRect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

  // Draw the scaled Image centered in the contents rectangle.
  targetRect_ = scaledPixmap_.rect();
  targetRect_.moveCenter( contentsRect().center() );
}
