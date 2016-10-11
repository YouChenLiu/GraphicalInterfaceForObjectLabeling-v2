#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QPainter>
#include <QTableWidgetItem>

#include "ROI/ROIBase.h"
#include "ROIManager/ROIManager.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  m_Shape = Shapes::RECTANGLE;
  ui->viewer->setShape(m_Shape);
  ui->viewer->setDrawPreview(true);
  connect(ui->cbShowRect, SIGNAL(clicked(bool)), this, SLOT(drawROIs()));
  connect(ui->cbShowEllipse, SIGNAL(clicked(bool)), this, SLOT(drawROIs()));
  connect(ui->cbShowCircle, SIGNAL(clicked(bool)), this, SLOT(drawROIs()));
  connect(ui->radioRect, SIGNAL(clicked(bool)), this, SLOT(onShapeChanged(bool)));
  connect(ui->radioEllipse, SIGNAL(clicked(bool)), this, SLOT(onShapeChanged(bool)));
  connect(ui->radioCircle, SIGNAL(clicked(bool)), this, SLOT(onShapeChanged(bool)));
  connect(&m_ROIManager, SIGNAL(countChanged(const QList<QSharedPointer<ROIBase>>&)), this, SLOT(updateTable(const QList<QSharedPointer<ROIBase>>&)));
}

MainWindow::~MainWindow()
{
  xmlManager.closeFile();
  delete ui;
}

void MainWindow::on_zoomInButton_clicked()
{
  ui->scalingBar->setSliderPosition(ui->scalingBar->sliderPosition() + 1);
}

void MainWindow::on_zoomOutButton_clicked()
{
  ui->scalingBar->setSliderPosition(ui->scalingBar->sliderPosition() - 1);
}

void MainWindow::on_scalingBar_valueChanged(int value)
{
  auto geometry = ui->viewer->geometry();
  auto x = geometry.x(), y = geometry.y();
  auto width = DEFAULT_WIDTH * value, height = DEFAULT_HEIGHT * value;
  ui->viewer->setGeometry(x, y, width, height);
}

void MainWindow::on_viewer_clicked(int x, int y)
{
  auto list = m_ROIManager.hit(x, y);
  if (list.isEmpty()) {
    ui->viewer->setDrawPreview(true);
  } else {
    ui->viewer->setDrawPreview(false);
    for (const auto& roi : list) {
      ui->viewer->addModifyingROI(roi->sn(), roi->rect(), roi->shape());
    }
  }
  drawROIs();
}

void MainWindow::on_viewer_newROI(const QRect& rect)
{
  switch (m_Shape) {
  case Shapes::RECTANGLE:
    m_ROIManager.addRectROI(rect.x(), rect.y(), rect.width(), rect.height());
    break;
  case Shapes::ELLIPSE:
    m_ROIManager.addEllipROI(rect.x(), rect.y(), rect.width(), rect.height());
    break;
  case Shapes::CIRCLE:
    m_ROIManager.addCirROI(rect.x(), rect.y(), rect.width() < rect.height() ? rect.width() : rect.height());
    break;
  default:
    break;
  }

  drawROIs();
}

void MainWindow::on_viewer_adjustROI(unsigned int sn, const QRect& rect)
{
  auto roi = m_ROIManager.getROI(sn);
  if (roi.isNull()) {
    return;
  }
  roi->setRect(rect);
}

void MainWindow::onShapeChanged(bool)
{
  if (ui->radioRect->isChecked()) {
    m_Shape = Shapes::RECTANGLE;
  } else if (ui->radioEllipse->isChecked()) {
    m_Shape = Shapes::ELLIPSE;
  } else {
    m_Shape = Shapes::CIRCLE;
  }
  ui->viewer->setShape(m_Shape);
}

void MainWindow::updateTable(const QList<QSharedPointer<ROIBase>>& listpROIs)
{
  drawROIs();

  ui->ROIList->clearContents();

  if (listpROIs.count() == 0) {
    return;
  }

  ui->ROIList->setRowCount(listpROIs.count());

  int row = 0;
  for (const auto& roi : listpROIs) {
    ui->ROIList->setItem(row, 0, new QTableWidgetItem(QString::number(roi->sn())));
    ui->ROIList->setItem(row, 1, new QTableWidgetItem(QString::number(roi->x())));
    ui->ROIList->setItem(row, 2, new QTableWidgetItem(QString::number(roi->y())));
    ui->ROIList->setItem(row, 3, new QTableWidgetItem(QString::number(roi->width())));
    ui->ROIList->setItem(row, 4, new QTableWidgetItem(QString::number(roi->height())));
    ui->ROIList->setItem(row, 5, new QTableWidgetItem(roi->getShapeString()));
    ++row;
  }
}

void MainWindow::drawROIs() const
{
  QPixmap pixmap(640, 480);
  QPainter painter(&pixmap);
  m_ROIManager.drawROIs(painter, 1.0, ui->cbShowRect->isChecked(), ui->cbShowEllipse->isChecked(), ui->cbShowCircle->isChecked());
  ui->viewer->setPixmap(pixmap);
}

void MainWindow::on_btnDelete_clicked()
{
  const auto rois = m_ROIManager.getROIs();
  for (const auto& roi : rois) {
    if (roi->selected()) {
      ui->viewer->removeModifyingROI(roi->sn());
      m_ROIManager.removeROI(roi->sn());
    }
  }
}
