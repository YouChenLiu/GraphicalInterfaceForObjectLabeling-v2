#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "common.h"
#include "ResultManager/ResultManager.h"
#include "ROIManager/ROIManager.h"
#include "ImgIO/ImgIOBase.h"

class QTableWidgetItem;
class QPixmap;
class SeqDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public:
  void dragEnterEvent(QDragEnterEvent*);
  void dragMoveEvent(QDragMoveEvent*);
  void dragLeaveEvent(QDragLeaveEvent*);
  void dropEvent(QDropEvent*);

protected:
  void MainWindow::closeEvent(QCloseEvent* e);

private slots:
  void on_zoomInButton_clicked();

  void on_zoomOutButton_clicked();

  void on_scalingBar_valueChanged(int value);

  void on_viewer_clicked(int x, int y);

  void on_viewer_newROI(const QRect& rect);

  void on_viewer_adjustROI(unsigned int sn, const QRect& rect);

  void onShapeChanged(bool);

  void updateTable(const QList<QSharedPointer<const ROIBase> > listpROIs);

  void on_btnDelete_clicked();

  void drawROIs(void) const;

  void on_btnPlayPause_clicked();

  void on_btnPrevious_clicked();

  void on_btnNext_clicked();

  void on_ROIList_itemClicked(QTableWidgetItem *item);

  void on_actionSave_As_triggered();

  void on_actionXML_triggered();

  void on_actionSave_triggered();

  void on_actionSequence_triggered();

  void on_cbReadonly_clicked();

  void on_newSequence(QSharedPointer<ImgIOBase> pImgIO);

private:
  Ui::MainWindow *ui;
  ResultManager m_XMLManager;
  ROIManager m_ROIManager;
  Shapes m_Shape;
  QSharedPointer<ImgIOBase> m_pImgReader;
  bool m_bReadonly;
  SeqDialog* seqDialog;

private:
  void settingByXML(void);
};

#endif // MAINWINDOW_H
