#include "ImgIOBase.h"

#include <QImage>
#include <QPixmap>

ImgIOBase::ImgIOBase(unsigned int first, SrcType type, QObject *parent) : QObject(parent)
{
  m_Type = type;
  m_iCurrentNum = m_iFirstNum = first;
  m_iEndNum = first + 1;
}

QPixmap ImgIOBase::convertToPixmap(const cv::Mat& mat)
{
  static QVector<QRgb> sColorTable;
  QImage image;
  switch (mat.type()) {
  case CV_8UC4:    // 8-bit, 4 channel
    return QPixmap::fromImage(QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB32));
  case CV_8UC3:    // 8-bit, 3 channel
    return QPixmap::fromImage(QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888).rgbSwapped());
  case CV_8UC1:    // 8-bit, 1 channel
    // only create our color table once
    if (sColorTable.isEmpty()) {
      for (int i = 0; i < 256; ++i) {
        sColorTable.push_back(qRgb(i, i, i));
      }
    }
    image = QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Indexed8);
    image.setColorTable(sColorTable);
    return QPixmap::fromImage(image);
  default:
      qWarning("ASM::cvMatToQImage() - cv::Mat image type not handled in switch:");
      break;
  }

  return QPixmap();
}
