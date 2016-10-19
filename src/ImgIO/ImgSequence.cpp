#include "ImgSequence.h"
#include <QString>
#include <QFileInfo>
#include <QPixmap>
#include <QChar>

ImgSequence::ImgSequence(const QString& root, const QString& extName, const QString& prefix, unsigned int length, unsigned int first) : ImgIOBase(first, SrcType::IMAGE)
{
  m_sRoot = root;
  m_sExtName = extName;
  m_sPrefix = prefix;
  m_iPadding = length;
  m_iReadingFlag = cv::IMREAD_COLOR;

  m_Img = loadImg(first);
}

QPixmap ImgSequence::queryNext(void)
{
  incCurrentNum();
  m_Img = loadImg(currentNum());

  return m_Img;
}

QPixmap ImgSequence::queryPre(void)
{
  decCurrentNum();
  m_Img = loadImg(currentNum());
  return m_Img;
}

QPixmap ImgSequence::query(unsigned int num)
{
  m_Img = loadImg(num);
  setCurrentNum(num);
  return m_Img;
}

bool ImgSequence::hasNext(void)
{
  if (currentNum() < endNum() - 1) {
    return true;
  }

  QFileInfo fileInfo(getPath(endNum()));
  bool exist = fileInfo.exists() && fileInfo.isFile();
  if (exist) {
    incEndNum();
  }
  return exist;
}

QString ImgSequence::getPath(unsigned int num) const
{
  QString sNumber = QString("%1").arg(num, m_iPadding, 10, QChar('0'));
  return QString("%1%2%3.%4").arg(m_sRoot).arg(m_sPrefix).arg(sNumber).arg(m_sExtName);
}

QPixmap ImgSequence::loadImg(unsigned int num) const
{
  QString path = getPath(num);
  cv::Mat img = cv::imread(path.toStdString(), m_iReadingFlag);
  if (img.empty()) {
    return QPixmap();
  }

  return convertToPixmap(img);
}
