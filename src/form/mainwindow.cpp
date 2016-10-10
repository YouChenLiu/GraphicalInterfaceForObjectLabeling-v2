#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <opencv2/highgui.hpp>
#include "ROI/RectROI.h"
#include "ROIManager/ROIManager.h"
#include <QPainter>
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  m_Shape = Shapes::RECTANGLE;
  ui->viewer->setShape(m_Shape);
  ui->viewer->setDrawPreview(true);
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
    return;
  }

  ui->viewer->setDrawPreview(false);
  for (const auto& roi : list) {
    ui->viewer->addModifyingROI(roi->getSN(), roi->getQRect(), roi->getShape());
  }

  drawROIs();
}

void MainWindow::on_viewer_newROI(const QPoint& ptLT, const QPoint& ptRB)
{
  switch (m_Shape) {
  case Shapes::RECTANGLE:
    m_ROIManager.addRectROI(ptLT, ptRB);
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
  roi->setRegion(rect);
}

void MainWindow::drawROIs() const
{
  const auto& rois = m_ROIManager.getROIs();
  QPixmap pixmap(640, 480);
  QPainter painter(&pixmap);
  for (const auto& roi : rois) {
    if (!roi->getVisable()) {
      continue;
    }
    switch (roi->getShape()) {
    case Shapes::RECTANGLE:
      painter.drawRect(roi->getQRect());
      break;
    default:
      break;
    }
  }
  ui->viewer->setPixmap(pixmap);
}
