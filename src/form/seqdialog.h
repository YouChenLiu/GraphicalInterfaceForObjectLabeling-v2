#ifndef SEQDIALOG_H
#define SEQDIALOG_H

#include <QDialog>

class ImgIOBase;

namespace Ui {
class SeqDialog;
}

class SeqDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SeqDialog(QWidget *parent = 0);
  ~SeqDialog();

public:
  void dragEnterEvent(QDragEnterEvent*);
  void dragMoveEvent(QDragMoveEvent*);
  void dragLeaveEvent(QDragLeaveEvent*);
  void dropEvent(QDropEvent*);

  void clear(void);

signals:
  void newSequence(QSharedPointer<ImgIOBase> pSeq);

private:
  Ui::SeqDialog *ui;

private slots:
  void on_buttonBox_accepted();

  void on_btnBrowsing_clicked();

  void onSettingChanged();
};

#endif // SEQDIALOG_H
