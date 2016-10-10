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

  void on_viewer_newROI(const QPoint& ptLT, const QPoint& ptRB);

  void on_viewer_adjustROI(unsigned int sn, const QRect& rect);

private:
  Ui::MainWindow *ui;
  ResultManager xmlManager;
  ROIManager m_ROIManager;
  Shapes m_Shape;

private:
  void drawROIs(void) const;
};

#endif // MAINWINDOW_H
