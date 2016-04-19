/**
 * \file   CapturedPieceWidget.h
 * \author Louis Parkin (louis.parkin@stonethree.com)
 * \date   April 2016
 * This file contains the inner class and member definitions of the CapturedPieceWidget class.
 */

#ifndef CAPTUREDPIECEWIDGET_H
#define CAPTUREDPIECEWIDGET_H

#include <QWidget>


namespace Ui
{
  /**
   * Forward declaration of the CapturedPieceWidget Ui class.
   */
  class CapturedPieceWidget;
}

/**
 * The CapturedPieceWidget class represents a normal QWidget with a PixmapWidget embedded for displaying graphics.
 */
class CapturedPieceWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * CapturedPieceWidget is the default constructor for a CapturedPieceWidget.
   * \param pixmap is the picture to render on the embedded PixmapWidget
   * \param parent represents the object that will destroy instances of CapturedPieceWidget, if not null.
   */
  explicit CapturedPieceWidget(const QPixmap& pixmap, QWidget* parent = 0);

  /**
   * ~CapturedPieceWidget is the default destructor of class CapturedPieceWidget.
   */
  virtual ~CapturedPieceWidget();

  /**
  * setPixmap is a wrapper function that forwards the setPixmap() request to the embedded PixmapWidget.
  * \param pixmap is the QPixmap object that should contain an image of a captured Chess Piece.
  */
  void setPixmap(const QPixmap& pixmap);

private:
  /**
   * ui is a pointer to the Ui::CapturedPieceWidget class, where all the graphical components reside.
   */
  Ui::CapturedPieceWidget* ui;
};

#endif // CAPTUREDPIECEWIDGET_H
