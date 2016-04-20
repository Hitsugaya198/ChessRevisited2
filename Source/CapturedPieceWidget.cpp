///
/// \file   CapturedPieceWidget.cpp
/// \author Louis Parkin (louis.parkin@stonethree.com)
/// \date   April 2016
/// This file wraps a PixmapWidget, specifically for displaying captured pieces.
///
/// This class exists only as a result of Qt Creator's ui designer not allowing to promote a
/// standalone QWidget directly to a PixmapWidget.  For that reason, the standalone QWidget Object
/// contains another QWidget name _pixmap, and this QWidget is then promoted to a PixmapWidget.
///

#include "CapturedPieceWidget.h"
#include "ui_CapturedPieceWidget.h"

CapturedPieceWidget::CapturedPieceWidget(const QPixmap& pixmap, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CapturedPieceWidget)
{
  ui->setupUi(this);
  setPixmap(pixmap);
}

CapturedPieceWidget::~CapturedPieceWidget()
{
  delete ui;
}

void CapturedPieceWidget::setPixmap(const QPixmap& pixmap)
{
  ui->_pixmap->setPixmap(pixmap);
}
