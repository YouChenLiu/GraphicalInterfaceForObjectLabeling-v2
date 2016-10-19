#ifndef IMGIOBASE_H
#define IMGIOBASE_H

#include <QObject>
#include <QPixmap>
#include <QMap>
#include <QString>

#include <opencv2/highgui.hpp>

enum class SrcType
{
  IMAGE,
  VIDEO
};

const QMap<QString, SrcType> StringType = {
  {QString("ImgSeq"), SrcType::IMAGE},
  {QString("Video"), SrcType::VIDEO}
};

class ImgIOBase : public QObject
{
  Q_OBJECT
private:
  unsigned int m_iFirstNum;
  unsigned int m_iEndNum;
  unsigned int m_iCurrentNum;
  SrcType m_Type;

protected:
  QPixmap m_Img;

public:
  explicit ImgIOBase(unsigned int first, SrcType type, QObject *parent = 0);

public:
  auto firstNum(void) const
  {
    return m_iFirstNum;
  }

  void setFirstNum(unsigned int value)
  {
    m_iFirstNum = value;
  }

  auto endNum(void) const
  {
    return m_iEndNum;
  }

  void setEndNum(unsigned int value)
  {
    m_iEndNum = value;
  }

  auto currentNum(void) const {
    return m_iCurrentNum;
  }

  auto srcType(void) const
  {
    return m_Type;
  }

  virtual QString srcTypeStr(void) const = 0;

  virtual QPixmap queryNext(void) = 0;

  virtual QPixmap queryPre(void) = 0;

  virtual QPixmap query(unsigned int num) = 0;

  virtual QPixmap getFrame(void) const
  {
    return m_Img;
  }

  virtual bool hasNext(void) = 0;

  bool hasFrame(unsigned int num) const
  {
    return (m_iFirstNum <= num) && (num < m_iEndNum);
  }

  bool hasPrevious(void) const
  {
    return m_iCurrentNum <= m_iFirstNum ? false : true;
  }

protected:
  void incEndNum(void)
  {
    ++m_iEndNum;
  }

  void incCurrentNum(void)
  {
    ++m_iCurrentNum;
  }

  void decCurrentNum(void)
  {
    --m_iCurrentNum;
  }

  void setCurrentNum(unsigned int num)
  {
    m_iCurrentNum = num;
  }

  static QPixmap convertToPixmap(const cv::Mat& mat);

signals:

public slots:
};

#endif // IMGIOBASE_H
