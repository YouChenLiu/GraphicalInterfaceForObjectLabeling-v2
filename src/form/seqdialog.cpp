#include "seqdialog.h"
#include "ui_seqdialog.h"

#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QRegExp>

#include "ImgIO/ImgSequence.h"

SeqDialog::SeqDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SeqDialog)
{
  ui->setupUi(this);
  connect(ui->textPath, SIGNAL(textChanged(QString)), this, SLOT(onSettingChanged()));
  connect(ui->textPrefix, SIGNAL(textChanged(QString)), this, SLOT(onSettingChanged()));
  connect(ui->spinLength, SIGNAL(valueChanged(int)), this, SLOT(onSettingChanged()));
  connect(ui->spinFirstNum, SIGNAL(valueChanged(int)), this, SLOT(onSettingChanged()));
  connect(ui->comboExt, SIGNAL(currentTextChanged(QString)), this, SLOT(onSettingChanged()));
}

SeqDialog::~SeqDialog()
{
  delete ui;
}

void SeqDialog::on_buttonBox_accepted()
{
  QSharedPointer<ImgIOBase> pSeq;
  QString root = ui->textPath->text().append(tr("/"));
  QString prefix = ui->textPrefix->text();
  int length = ui->spinLength->value();
  int first = ui->spinFirstNum->value();
  QString ext = ui->comboExt->currentText();
  pSeq = QSharedPointer<ImgSequence>::create(root, ext, prefix, length, first);
  emit newSequence(pSeq);
}

void SeqDialog::on_btnBrowsing_clicked()
{
  QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
  if (!path.isEmpty()) {
    ui->textPath->setText(path);
  }
}

void SeqDialog::onSettingChanged()
{
  ui->textPreview->clear();

  QString root = ui->textPath->text().append(tr("/"));
  QString prefix = ui->textPrefix->text();
  int length = ui->spinLength->value();
  int first = ui->spinFirstNum->value();
  QString ext = ui->comboExt->currentText();

  for (int i = first; i < first + 10; ++i) {
    QString path = root + prefix + QString("%1.%2").arg(i, length, 10, QChar('0')).arg(ext);
    ui->textPreview->append(path);
  }
}

void SeqDialog::dragEnterEvent(QDragEnterEvent* e)
{
  // if some actions should not be usable, like move, this code must be adopted
  e->acceptProposedAction();
}

void SeqDialog::dragMoveEvent(QDragMoveEvent* e)
{
  // if some actions should not be usable, like move, this code must be adopted
  e->acceptProposedAction();
}

void SeqDialog::dragLeaveEvent(QDragLeaveEvent* e)
{
  e->accept();
}

void SeqDialog::dropEvent(QDropEvent* e)
{
  const QMimeData* mimeData = e->mimeData();

  if (mimeData->hasUrls()) {
    // get dropped url
    QList<QUrl> urlList = mimeData->urls();
    QString filePath = urlList.at(0).toLocalFile();

    // separate the directory by finding last slash
    int lastSlah = filePath.lastIndexOf(QChar('/'));
    QString root = filePath.left(lastSlah);
    ui->textPath->setText(root);

    // separate the file name include extension name
    QString fileName = filePath.right(filePath.length() - lastSlah - 1);

    // separate extension name by last period
    int period = fileName.lastIndexOf(QChar('.'));
    QString ext = fileName.mid(period + 1);
    ui->comboExt->setCurrentText(ext);

    // separate filename
    fileName = fileName.left(period);

    // find the number by regular expression
    QRegExp exp("(\\d+)");
    int pos = exp.indexIn(fileName);

    // check the nmber string position
    if (pos == 0) {
      // this string is not have prefix
      ui->textPrefix->clear();
    } else {
      QString prefix = fileName.left(pos);
      ui->textPrefix->setText(prefix);
    }

    // separate number string
    QString number = fileName.mid(pos);
    ui->spinFirstNum->setValue(number.toInt());
    ui->spinLength->setValue(number.length());
  }
}
