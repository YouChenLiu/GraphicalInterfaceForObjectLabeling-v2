#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "common.h"
#include "ResultManager/ResultManager.h"
#include "ROIManager/ROIManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

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

private:
  Ui::MainWindow *ui;
  ResultManager xmlManager;
  ROIManager m_ROIManager;
  Shapes m_Shape;

private:

};

#endif // MAINWINDOW_H
