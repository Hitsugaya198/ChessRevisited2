#include "CapturedPieceWidget.h"
#include "ui_CapturedPieceWidget.h"

CapturedPieceWidget::CapturedPieceWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CapturedPieceWidget)
{
  ui->setupUi(this);
}

CapturedPieceWidget::~CapturedPieceWidget()
{
  delete ui;
}
