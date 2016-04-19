/**
 * @file   CapturedPieceWidget.cpp
 * @author Louis Parkin (louis.parkin@stonethree.com)
 * @date   April 2016
 * @brief  This file wraps a PixmapWidget, specifically for displaying captured pieces.
 *
 * This class exists only as a result of Qt Creator's ui designer not allowing to promote a
 * standalone QWidget directly to a PixmapWidget.  For that reason, the standalone QWidget Object
 * contains another QWidget name _pixmap, and this QWidget is then promoted to a PixmapWidget.
 */

#include "CapturedPieceWidget.h"
#include "ui_CapturedPieceWidget.h"

///
/// \brief CapturedPieceWidget::CapturedPieceWidget() is the default constructor for a CapturedPieceWidget.
/// \param pixmap is the picture to render on the embedded PixmapWidget
/// \param parent represents the object that will destroy instances of CapturedPieceWidget, if not null.
///
CapturedPieceWidget::CapturedPieceWidget(const QPixmap& pixmap, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CapturedPieceWidget)
{
  ui->setupUi(this);
  setPixmap(pixmap);
}

///
/// \brief CapturedPieceWidget::~CapturedPieceWidget() is the default destructor of class CapturedPieceWidget.
///
CapturedPieceWidget::~CapturedPieceWidget()
{
  delete ui;
}

///
/// \brief CapturedPieceWidget::setPixmap() is a wrapper function that forwards the setPixmap() request to the embedded PixmapWidget.
/// \param pixmap is the QPixmap object that should contain an image of a captured Chess Piece.
///
void CapturedPieceWidget::setPixmap(const QPixmap& pixmap)
{
  ui->_pixmap->setPixmap(pixmap);
}
