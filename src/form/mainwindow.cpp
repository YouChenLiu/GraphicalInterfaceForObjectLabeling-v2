#include "mainwindow.h"
#include "seqdialog.h"
#include "ui_mainwindow.h"
#include "ui_seqdialog.h"
#include <QKeyEvent>
#include <QPixmap>
#include <QPainter>
#include <QTableWidgetItem>
#include <QList>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "ROI/ROIBase.h"
#include "ROIManager/ROIManager.h"
#include "ImgIO/ImgSequence.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // setting table width
  ui->ROIList->setColumnWidth(0, 30);
  ui->ROIList->setColumnWidth(1, 40);
  ui->ROIList->setColumnWidth(2, 40);
  ui->ROIList->setColumnWidth(3, 50);
  ui->ROIList->setColumnWidth(4, 50);
  ui->ROIList->setColumnWidth(5, 70);

  // initial value
  m_Shape = Shapes::RECTANGLE;
  ui->viewer->setShape(m_Shape);
  m_bReadonly = false;
  ui->viewer->setDrawPreview(false);
  seqDialog = new SeqDialog(this);

  // connect signal and slot
  connect(ui->cbShowRect, SIGNAL(clicked(bool)), this, SLOT(drawROIs()));
  connect(ui->cbShowEllipse, SIGNAL(clicked(bool)), this, SLOT(drawROIs()));
  connect(ui->cbShowCircle, SIGNAL(clicked(bool)), this, SLOT(drawROIs()));
  connect(ui->radioRect, SIGNAL(clicked(bool)), this, SLOT(onShapeChanged(bool)));
  connect(ui->radioEllipse, SIGNAL(clicked(bool)), this, SLOT(onShapeChanged(bool)));
  connect(ui->radioCircle, SIGNAL(clicked(bool)), this, SLOT(onShapeChanged(bool)));
  connect(&m_ROIManager, SIGNAL(onListChanged(QList<QSharedPointer<const ROIBase>>)), this, SLOT(updateTable(const QList<QSharedPointer<const ROIBase>>)));
  connect(seqDialog, SIGNAL(newSequence(QSharedPointer<ImgIOBase>)), SLOT(on_newSequence(QSharedPointer<ImgIOBase>)));
}

MainWindow::~MainWindow()
{

  delete ui;
}

void MainWindow::settingByXML()
{
  QString sType = m_XMLManager.type();
  if (StringType[sType] == SrcType::IMAGE) {
    int firstNum = m_XMLManager.firstNum();
    int endNum = m_XMLManager.endNum();
    int length = m_XMLManager.paddingLength();
    QString ext = m_XMLManager.extension();
    QString path = m_XMLManager.filePath();
    QString root = path.left(path.lastIndexOf(QChar('/')) + 1);
    QSharedPointer<ImgSequence> seq = QSharedPointer<ImgSequence>::create(root, ext, tr(""), length, firstNum);
    seq->setEndNum(endNum);
    m_pImgReader = seq;
  }

  m_ROIManager.setROIs(m_XMLManager.getROIs(m_pImgReader->currentNum()));
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
  // if some actions should not be usable, like move, this code must be adopted
  e->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* e)
{
  // if some actions should not be usable, like move, this code must be adopted
  e->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* e)
{
  e->accept();
}

void MainWindow::dropEvent(QDropEvent* e)
{
  const QMimeData* mimeData = e->mimeData();

  if (mimeData->hasUrls()) {
    // get dropped url
    QList<QUrl> urlList = mimeData->urls();
    QString filePath = urlList.at(0).toLocalFile();
    m_XMLManager.openFile(filePath);
    settingByXML();
  }
}

void MainWindow::closeEvent(QCloseEvent* e)
{
  IOResult fileState = m_XMLManager.closeFile();
  if (fileState == IOResult::FILE_NOT_SAVED) {
    auto selection = QMessageBox::question(this, tr("File not save"), tr("You are NOT save the changes of result.\nDo you want to DISCARD it?"));
    if (selection == QMessageBox::StandardButton::Yes) {
      m_XMLManager.closeFile(true);
    } else {
      e->ignore();
    }
  }
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
  //ui->viewer->setGeometry(x, y, width, height);
}

void MainWindow::on_viewer_clicked(int x, int y)
{
  const auto list = m_ROIManager.hit(x, y);
  if (list.isEmpty()) {
    ui->viewer->setDrawPreview(true);
  } else {
    ui->viewer->clearModifyingROI();
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
}

void MainWindow::on_viewer_adjustROI(unsigned int sn, const QRect& rect)
{
  m_ROIManager.adjustROI(sn, rect);
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

void MainWindow::updateTable(const QList<QSharedPointer<const ROIBase>> listpROIs)
{
  drawROIs();

  ui->ROIList->clearContents();
  ui->ROIList->setRowCount(listpROIs.count());

  if (listpROIs.count() == 0) {
    return;
  }

  int row = 0;
  for (const auto& roi : listpROIs) {
    QVector<QTableWidgetItem*> vpItem;
    vpItem.append(new QTableWidgetItem(QString::number(roi->sn())));
    vpItem.append(new QTableWidgetItem(QString::number(roi->x())));
    vpItem.append(new QTableWidgetItem(QString::number(roi->y())));
    vpItem.append(new QTableWidgetItem(QString::number(roi->width())));
    vpItem.append(new QTableWidgetItem(QString::number(roi->height())));
    vpItem.append(new QTableWidgetItem(roi->getShapeString()));

    auto wrong = [&](void) {
      if (roi->x() < 0 || roi->x() > ui->viewer->width()) return true;
      if (roi->y() < 0 || roi->y() > ui->viewer->height()) return true;
      if (roi->width() == 0 || roi->height() == 0) return true;
      return false;
    }();

    for (int i = 0; i < vpItem.size(); ++i) {
      if (wrong) {
        vpItem[i]->setBackgroundColor(Qt::red);
      }

      ui->ROIList->setItem(row, i, vpItem.at(i));
    }
    ++row;
  }
}

void MainWindow::drawROIs(void) const
{
  QPixmap pixmap = m_pImgReader->getFrame();
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

void MainWindow::on_btnPlayPause_clicked()
{

}

void MainWindow::on_btnPrevious_clicked()
{
  if (m_pImgReader->hasPrevious()) {
    m_XMLManager.addROIs(m_pImgReader->currentNum(), m_ROIManager.getROIs());
    m_XMLManager.saveTempFile();
    m_pImgReader->queryPre();
    m_ROIManager.setROIs(m_XMLManager.getROIs(m_pImgReader->currentNum()));
  }
}

void MainWindow::on_btnNext_clicked()
{
  if (m_pImgReader->hasNext()) {
    m_XMLManager.addROIs(m_pImgReader->currentNum(), m_ROIManager.getROIs());
    m_XMLManager.saveTempFile();
    m_pImgReader->queryNext();
    if (m_XMLManager.hasFrame(m_pImgReader->currentNum())) {
      m_ROIManager.setROIs(m_XMLManager.getROIs(m_pImgReader->currentNum()));
    } else {
      m_ROIManager.reset();
    }
  }
}

void MainWindow::on_ROIList_itemClicked(QTableWidgetItem *item)
{
  m_ROIManager.clearROIState();
  auto row = item->row();
  auto sn = ui->ROIList->item(row, 0)->text().toInt();
  auto roi = m_ROIManager.getConstROI(sn);
  ui->viewer->clearModifyingROI();
  ui->viewer->addModifyingROI(sn, roi->rect(), roi->shape());
  m_ROIManager.selectROI(sn);
}

void MainWindow::on_actionSave_As_triggered()
{
  m_XMLManager.saveAs();
}


void MainWindow::on_actionXML_triggered()
{
  m_XMLManager.openFile();
}

void MainWindow::on_actionSave_triggered()
{
  m_XMLManager.saveFile();
}

void MainWindow::on_actionSequence_triggered()
{
  seqDialog->show();
}

void MainWindow::on_cbReadonly_clicked()
{
  m_bReadonly = ui->cbReadonly->isChecked();
  ui->viewer->setDrawPreview(!m_bReadonly);
}

void MainWindow::on_newSequence(QSharedPointer<ImgIOBase> pImgIO)
{
  if (pImgIO.isNull()) {
    QMessageBox::information(this, tr("Image IO ERROR"), tr("Invalid image sequence."));
    return;
  }
  m_pImgReader = pImgIO;
}
