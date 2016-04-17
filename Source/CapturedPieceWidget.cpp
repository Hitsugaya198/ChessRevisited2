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
